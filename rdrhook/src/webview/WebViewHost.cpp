#include "stdafx.h"

#include "WebViewHost.h"

// WebView2's generated MIDL header assumes the MSVC `interface` extension.
// clang-cl does not provide it when windows.h is included with our lean flags.
#ifndef interface
#define interface struct
#endif
#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>
#include <DispatcherQueue.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>

#include <d3d11_4.h>
#include <dxgi1_4.h>
#include <windowsx.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/base.h>

#include <wrl.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace rdr2::webview {
namespace {

using Microsoft::WRL::Callback;
using Microsoft::WRL::ComPtr;

std::wstring ToWide(std::string_view value)
{
    if (value.empty()) return {};
    const int length = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS, value.data(),
        static_cast<int>(value.size()), nullptr, 0);
    if (length <= 0) return {};

    std::wstring result(static_cast<std::size_t>(length), L'\0');
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(),
                            static_cast<int>(value.size()), result.data(),
                            length) != length) {
        return {};
    }
    return result;
}

std::string ToUtf8(std::wstring_view value)
{
    if (value.empty()) return {};
    const int length = WideCharToMultiByte(
        CP_UTF8, WC_ERR_INVALID_CHARS, value.data(),
        static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    if (length <= 0) return {};

    std::string result(static_cast<std::size_t>(length), '\0');
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value.data(),
                            static_cast<int>(value.size()), result.data(),
                            length, nullptr, nullptr) != length) {
        return {};
    }
    return result;
}

COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS MouseVirtualKeys(WPARAM wParam)
{
    unsigned int keys = COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_NONE;
    if (wParam & MK_LBUTTON)
        keys |= COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_LEFT_BUTTON;
    if (wParam & MK_RBUTTON)
        keys |= COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_RIGHT_BUTTON;
    if (wParam & MK_MBUTTON)
        keys |= COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_MIDDLE_BUTTON;
    if (wParam & MK_XBUTTON1)
        keys |= COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_X_BUTTON1;
    if (wParam & MK_XBUTTON2)
        keys |= COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_X_BUTTON2;
    if (wParam & MK_SHIFT)
        keys |= COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_SHIFT;
    if (wParam & MK_CONTROL)
        keys |= COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_CONTROL;
    return static_cast<COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS>(keys);
}

struct MouseCommand {
    COREWEBVIEW2_MOUSE_EVENT_KIND kind{};
    COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS keys{};
    std::uint32_t data = 0;
    POINT point{};
};

} // namespace

struct CapturedFrameState {
    ComPtr<ID3D11Texture2D> texture;
    HANDLE sharedHandle = nullptr;
    std::atomic_uint64_t consumerFenceValue = 0;
    std::weak_ptr<const CapturedFrame> descriptor;
    std::uint32_t width = 0;
    std::uint32_t height = 0;

    ~CapturedFrameState()
    {
        if (sharedHandle) CloseHandle(sharedHandle);
    }
};

CapturedFrame::CapturedFrame(HANDLE sharedHandle,
                             HANDLE producerFenceHandle,
                             HANDLE consumerFenceHandle,
                             std::uint32_t width, std::uint32_t height,
                             std::uint64_t generation,
                             std::uint64_t sequence,
                             std::uint64_t producerFenceValue,
                             std::shared_ptr<CapturedFrameState> state)
    : sharedHandle_(sharedHandle),
      producerFenceHandle_(producerFenceHandle),
      consumerFenceHandle_(consumerFenceHandle),
      width_(width),
      height_(height),
      generation_(generation),
      sequence_(sequence),
      producerFenceValue_(producerFenceValue),
      state_(std::move(state))
{
}

void CapturedFrame::MarkSubmitted(std::uint64_t consumerFenceValue) const
{
    if (!state_) return;
    std::uint64_t current =
        state_->consumerFenceValue.load(std::memory_order_relaxed);
    while (current < consumerFenceValue &&
           !state_->consumerFenceValue.compare_exchange_weak(
               current, consumerFenceValue, std::memory_order_release,
               std::memory_order_relaxed)) {
    }
}

struct WebViewHost::Impl {
    ~Impl() { StopAndJoin(); }

    bool Start(HWND parentWindow, LUID adapterLuid,
               std::wstring userDataFolder, std::string initialUrl,
               std::uint32_t width, std::uint32_t height)
    {
        if (running.load(std::memory_order_acquire)) return false;
        // Closing a composition controller may need the game window thread to
        // process messages. Never wait for that worker here. Reap it only once
        // it has reported that teardown is complete.
        if (!TryReapStoppedThread()) {
            spdlog::warn(
                "[WebView2] Previous composition browser is still closing");
            return false;
        }

        std::scoped_lock lock(lifecycleMutex);
        if (running.load(std::memory_order_acquire)) return false;
        if (!parentWindow || width == 0 || height == 0) return false;

        this->parentWindow = parentWindow;
        this->adapterLuid = adapterLuid;
        this->userDataFolder = std::move(userDataFolder);
        desiredUrl = std::move(initialUrl);
        desiredWidth = width;
        desiredHeight = height;
        desiredFocused = false;
        ++generation;
        sequence = 0;
        producerFenceValue = 0;
        captureCallbacks = 0;
        capturedFrames = 0;
        droppedFrames = 0;
        captureStatsStarted = std::chrono::steady_clock::now();
        focused.store(false, std::memory_order_release);
        visible.store(true, std::memory_order_release);
        focusReleaseRequested.store(false, std::memory_order_release);
        pendingJson.clear();
        webMessageToken = {};
        acceleratorToken = {};
        {
            std::scoped_lock messageLock(messageMutex);
            messages.clear();
        }

        commandEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!commandEvent) return false;

        threadExited.store(false, std::memory_order_release);
        running.store(true, std::memory_order_release);
        thread = std::jthread([this](std::stop_token stopToken) {
            ThreadMain(stopToken);
        });
        return true;
    }

    void Stop()
    {
        const bool wasRunning =
            running.exchange(false, std::memory_order_acq_rel);
        visible.store(false, std::memory_order_release);
        focused.store(false, std::memory_order_release);
        {
            std::scoped_lock lock(lifecycleMutex);
            if (thread.joinable()) thread.request_stop();
            if (commandEvent) SetEvent(commandEvent);
        }
        if (wasRunning)
            spdlog::info("[WebView2] Composition browser shutdown requested");
    }

    bool TryReapStoppedThread()
    {
        std::jthread stoppedThread;
        HANDLE eventToClose = nullptr;
        {
            std::scoped_lock lock(lifecycleMutex);
            if (thread.joinable()) {
                if (!threadExited.load(std::memory_order_acquire)) return false;
                stoppedThread = std::move(thread);
            }
            eventToClose = std::exchange(commandEvent, nullptr);
        }

        // The exit flag is published at the very end of ThreadMain, so this
        // join cannot wait on WebView2 or WGC teardown.
        if (stoppedThread.joinable()) stoppedThread.join();
        if (eventToClose) CloseHandle(eventToClose);
        ready.store(false, std::memory_order_release);
        latestFrame.store(nullptr, std::memory_order_release);
        {
            std::scoped_lock lock(commandMutex);
            commands.clear();
        }
        return true;
    }

    void StopAndJoin()
    {
        Stop();
        std::jthread stoppedThread;
        HANDLE eventToClose = nullptr;
        {
            std::scoped_lock lock(lifecycleMutex);
            stoppedThread = std::move(thread);
            eventToClose = std::exchange(commandEvent, nullptr);
        }
        if (stoppedThread.joinable()) stoppedThread.join();
        if (eventToClose) CloseHandle(eventToClose);
    }

    void Post(std::function<void()> command)
    {
        if (!running.load(std::memory_order_acquire)) return;
        {
            std::scoped_lock lock(commandMutex);
            commands.push_back(std::move(command));
        }
        HANDLE event = nullptr;
        {
            std::scoped_lock lock(lifecycleMutex);
            event = commandEvent;
            if (event) SetEvent(event);
        }
    }

    void DrainCommands()
    {
        std::deque<std::function<void()>> pending;
        {
            std::scoped_lock lock(commandMutex);
            pending.swap(commands);
        }
        for (auto& command : pending) command();
    }

    void ThreadMain(std::stop_token stopToken) noexcept
    {
        try {
            ThreadMainImpl(stopToken);
        } catch (const winrt::hresult_error& error) {
            spdlog::error(
                "[WebView2] Unhandled WinRT error on browser thread: "
                "0x{:08X}",
                static_cast<unsigned int>(error.code()));
            running.store(false, std::memory_order_release);
            threadExited.store(true, std::memory_order_release);
        } catch (const std::exception& error) {
            spdlog::error(
                "[WebView2] Unhandled browser-thread exception: {}",
                error.what());
            running.store(false, std::memory_order_release);
            threadExited.store(true, std::memory_order_release);
        } catch (...) {
            spdlog::error(
                "[WebView2] Unhandled unknown browser-thread exception");
            running.store(false, std::memory_order_release);
            threadExited.store(true, std::memory_order_release);
        }
    }

    void ThreadMainImpl(std::stop_token stopToken)
    {
        struct ExitNotifier final {
            std::atomic_bool& exited;
            ~ExitNotifier()
            {
                exited.store(true, std::memory_order_release);
            }
        } exitNotifier{threadExited};

        winrt::init_apartment(winrt::apartment_type::single_threaded);

        DispatcherQueueOptions options{
            sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT,
            DQTAT_COM_STA};
        const HRESULT queueResult = CreateDispatcherQueueController(
            options,
            reinterpret_cast<ABI::Windows::System::
                IDispatcherQueueController**>(
                winrt::put_abi(dispatcherQueueController)));
        if (FAILED(queueResult)) {
            spdlog::error(
                "[WebView2] Failed to create dispatcher queue: 0x{:08X}",
                static_cast<unsigned int>(queueResult));
            running.store(false, std::memory_order_release);
            winrt::uninit_apartment();
            return;
        }

        if (!CreateCaptureDevice() || !BeginCreateWebView()) {
            CleanupOnThread();
            running.store(false, std::memory_order_release);
            winrt::uninit_apartment();
            return;
        }

        bool quitRequested = false;
        while (!stopToken.stop_requested() && !quitRequested) {
            HANDLE event = nullptr;
            {
                std::scoped_lock lock(lifecycleMutex);
                event = commandEvent;
            }
            if (!event) break;

            const DWORD waitResult = MsgWaitForMultipleObjectsEx(
                1, &event, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (waitResult == WAIT_OBJECT_0) DrainCommands();
            if (waitResult == WAIT_OBJECT_0 + 1) {
                MSG message{};
                while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                    if (message.message == WM_QUIT) {
                        quitRequested = true;
                        break;
                    }
                    TranslateMessage(&message);
                    DispatchMessageW(&message);
                }
            }
        }

        DrainCommands();
        CleanupOnThread();
        running.store(false, std::memory_order_release);
        winrt::uninit_apartment();
        spdlog::info("[WebView2] Composition browser stopped");
    }

    bool CreateCaptureDevice()
    {
        ComPtr<IDXGIFactory4> factory;
        HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
        if (FAILED(result)) return false;

        ComPtr<IDXGIAdapter> adapter;
        result = factory->EnumAdapterByLuid(adapterLuid,
                                            IID_PPV_ARGS(&adapter));
        if (FAILED(result)) {
            spdlog::error(
                "[WebView2] Could not find RDR2's graphics adapter: 0x{:08X}",
                static_cast<unsigned int>(result));
            return false;
        }
        constexpr D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
        constexpr UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT |
                               D3D11_CREATE_DEVICE_VIDEO_SUPPORT;

        D3D_FEATURE_LEVEL selectedLevel{};
        result = D3D11CreateDevice(
            adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags,
            featureLevels, static_cast<UINT>(std::size(featureLevels)),
            D3D11_SDK_VERSION, &d3d11Device, &selectedLevel, &d3d11Context);
        if (FAILED(result)) {
            spdlog::error(
                "[WebView2] Failed to create the capture device: 0x{:08X}",
                static_cast<unsigned int>(result));
            return false;
        }
        ComPtr<ID3D10Multithread> multithread;
        if (SUCCEEDED(d3d11Device.As(&multithread)))
            multithread->SetMultithreadProtected(TRUE);

        result = d3d11Device.As(&d3d11Device5);
        if (FAILED(result)) {
            spdlog::error(
                "[WebView2] ID3D11Device5 is required for shared fences: "
                "0x{:08X}",
                static_cast<unsigned int>(result));
            return false;
        }
        result = d3d11Context.As(&d3d11Context4);
        if (FAILED(result)) {
            spdlog::error(
                "[WebView2] ID3D11DeviceContext4 is required for shared "
                "fences: 0x{:08X}",
                static_cast<unsigned int>(result));
            return false;
        }

        result = d3d11Device5->CreateFence(
            0, D3D11_FENCE_FLAG_SHARED, IID_PPV_ARGS(&producerFence));
        if (FAILED(result)) return false;
        result = producerFence->CreateSharedHandle(
            nullptr, GENERIC_ALL, nullptr, &producerFenceHandle);
        if (FAILED(result)) return false;

        result = d3d11Device5->CreateFence(
            0, D3D11_FENCE_FLAG_SHARED, IID_PPV_ARGS(&consumerFence));
        if (FAILED(result)) return false;
        result = consumerFence->CreateSharedHandle(
            nullptr, GENERIC_ALL, nullptr, &consumerFenceHandle);
        if (FAILED(result)) return false;

        ComPtr<IDXGIDevice> dxgiDevice;
        result = d3d11Device.As(&dxgiDevice);
        if (FAILED(result)) return false;

        winrt::com_ptr<IInspectable> inspectableDevice;
        result = CreateDirect3D11DeviceFromDXGIDevice(
            dxgiDevice.Get(), inspectableDevice.put());
        if (FAILED(result)) return false;

        direct3DDevice = inspectableDevice.as<
            winrt::Windows::Graphics::DirectX::Direct3D11::
                IDirect3DDevice>();
        return direct3DDevice != nullptr;
    }

    bool BeginCreateWebView()
    {
        const std::uint32_t refreshRate = DisplayRefreshRate();
        const std::wstring browserArguments =
            L"--disable-gpu-vsync --max-gum-fps=" +
            std::to_wstring(refreshRate);
        environmentOptions =
            Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
        if (!environmentOptions ||
            FAILED(environmentOptions->put_AdditionalBrowserArguments(
                browserArguments.c_str()))) {
            spdlog::error(
                "[WebView2] Could not configure offscreen frame pacing");
            return false;
        }
        spdlog::info(
            "[WebView2] Chromium offscreen pacing target: {} Hz",
            refreshRate);

        const HRESULT result = CreateCoreWebView2EnvironmentWithOptions(
            nullptr,
            userDataFolder.empty() ? nullptr : userDataFolder.c_str(),
            environmentOptions.Get(),
            Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
                [this](HRESULT status,
                       ICoreWebView2Environment* createdEnvironment) {
                    if (FAILED(status) || !createdEnvironment) {
                        spdlog::error(
                            "[WebView2] Environment creation failed: 0x{:08X}",
                            static_cast<unsigned int>(status));
                        return status;
                    }

                    environment = createdEnvironment;
                    ComPtr<ICoreWebView2Environment3> compositionEnvironment;
                    HRESULT queryResult =
                        environment.As(&compositionEnvironment);
                    if (FAILED(queryResult)) return queryResult;

                    return compositionEnvironment
                        ->CreateCoreWebView2CompositionController(
                            parentWindow,
                            Callback<ICoreWebView2CreateCoreWebView2CompositionControllerCompletedHandler>(
                                [this](HRESULT controllerStatus,
                                       ICoreWebView2CompositionController*
                                           createdController) {
                                    return OnControllerCreated(
                                        controllerStatus, createdController);
                                })
                                .Get());
                })
                .Get());

        if (FAILED(result)) {
            spdlog::error(
                "[WebView2] Could not start environment creation: 0x{:08X}",
                static_cast<unsigned int>(result));
            return false;
        }
        return true;
    }

    std::uint32_t DisplayRefreshRate() const
    {
        MONITORINFOEXW monitorInfo{};
        monitorInfo.cbSize = sizeof(monitorInfo);
        const HMONITOR monitor =
            MonitorFromWindow(parentWindow, MONITOR_DEFAULTTONEAREST);
        if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
            DEVMODEW mode{};
            mode.dmSize = sizeof(mode);
            if (EnumDisplaySettingsExW(monitorInfo.szDevice,
                                       ENUM_CURRENT_SETTINGS, &mode, 0) &&
                mode.dmDisplayFrequency >= 30 &&
                mode.dmDisplayFrequency <= 1000) {
                return mode.dmDisplayFrequency;
            }
        }
        return 60;
    }

    HRESULT OnControllerCreated(
        HRESULT status,
        ICoreWebView2CompositionController* createdCompositionController)
    {
        if (FAILED(status) || !createdCompositionController) {
            spdlog::error(
                "[WebView2] Composition controller creation failed: 0x{:08X}",
                static_cast<unsigned int>(status));
            return status;
        }

        compositionController = createdCompositionController;
        HRESULT result = compositionController.As(&controller);
        if (FAILED(result)) return result;
        result = controller->get_CoreWebView2(&webview);
        if (FAILED(result)) return result;

        ComPtr<ICoreWebView2Controller2> controller2;
        if (SUCCEEDED(controller.As(&controller2))) {
            const COREWEBVIEW2_COLOR transparent{0, 0, 0, 0};
            controller2->put_DefaultBackgroundColor(transparent);
        }

        result = CreateCompositionCapture();
        if (FAILED(result)) return result;

        result = webview->add_WebMessageReceived(
            Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                [this](ICoreWebView2*,
                       ICoreWebView2WebMessageReceivedEventArgs* args) {
                    LPWSTR json = nullptr;
                    const HRESULT messageResult =
                        args->get_WebMessageAsJson(&json);
                    if (SUCCEEDED(messageResult) && json) {
                        std::string message = ToUtf8(json);
                        CoTaskMemFree(json);
                        std::scoped_lock lock(messageMutex);
                        messages.push_back(std::move(message));
                    }
                    return S_OK;
                })
                .Get(),
            &webMessageToken);
        if (FAILED(result)) return result;

        result = controller->add_AcceleratorKeyPressed(
            Callback<ICoreWebView2AcceleratorKeyPressedEventHandler>(
                [this](ICoreWebView2Controller*,
                       ICoreWebView2AcceleratorKeyPressedEventArgs* args) {
                    UINT virtualKey = 0;
                    COREWEBVIEW2_KEY_EVENT_KIND kind{};
                    if (FAILED(args->get_VirtualKey(&virtualKey)) ||
                        FAILED(args->get_KeyEventKind(&kind)) ||
                        virtualKey != VK_F8 ||
                        (kind != COREWEBVIEW2_KEY_EVENT_KIND_KEY_DOWN &&
                         kind != COREWEBVIEW2_KEY_EVENT_KIND_SYSTEM_KEY_DOWN)) {
                        return S_OK;
                    }
                    args->put_Handled(TRUE);
                    focusReleaseRequested.store(true,
                                                std::memory_order_release);
                    return S_OK;
                })
                .Get(),
            &acceleratorToken);
        if (FAILED(result)) return result;

        ApplyBounds();
        controller->put_IsVisible(TRUE);
        ApplyNavigation();
        ApplyFocus();
        FlushPendingJson();

        ready.store(true, std::memory_order_release);
        spdlog::info("[WebView2] Composition browser initialized");
        return S_OK;
    }

    HRESULT CreateCompositionCapture()
    {
        try {
            compositor = winrt::Windows::UI::Composition::Compositor();
            rootVisual = compositor.CreateContainerVisual();
            webviewVisual = compositor.CreateContainerVisual();
            rootVisual.Size({static_cast<float>(desiredWidth),
                             static_cast<float>(desiredHeight)});
            webviewVisual.RelativeSizeAdjustment({1.0f, 1.0f});
            rootVisual.Children().InsertAtTop(webviewVisual);

            HRESULT result = compositionController->put_RootVisualTarget(
                winrt::get_unknown(webviewVisual));
            if (FAILED(result)) return result;

            captureItem = winrt::Windows::Graphics::Capture::
                GraphicsCaptureItem::CreateFromVisual(rootVisual);
            framePool = winrt::Windows::Graphics::Capture::
                Direct3D11CaptureFramePool::Create(
                    direct3DDevice,
                    winrt::Windows::Graphics::DirectX::
                        DirectXPixelFormat::B8G8R8A8UIntNormalized,
                    2, captureItem.Size());
            frameArrivedToken = framePool.FrameArrived(
                [this](auto const&, auto const&) { CaptureFrame(); });
            captureSession = framePool.CreateCaptureSession(captureItem);
            captureSession.StartCapture();
            return S_OK;
        } catch (const winrt::hresult_error& error) {
            return error.code();
        }
    }

    void CaptureFrame()
    {
        try {
            auto frame = framePool.TryGetNextFrame();
            if (!frame) return;
            ++captureCallbacks;

            auto access = frame.Surface().as<
                ::Windows::Graphics::DirectX::Direct3D11::
                    IDirect3DDxgiInterfaceAccess>();
            if (!access) return;

            ComPtr<ID3D11Texture2D> texture;
            HRESULT result = access->GetInterface(IID_PPV_ARGS(&texture));
            if (FAILED(result)) return;

            D3D11_TEXTURE2D_DESC sourceDesc{};
            texture->GetDesc(&sourceDesc);
            if (surfaces.empty() || surfaceWidth != sourceDesc.Width ||
                surfaceHeight != sourceDesc.Height ||
                surfaceFormat != sourceDesc.Format) {
                if (!CreateSharedSurfaceRing(sourceDesc)) return;
            }

            std::shared_ptr<CapturedFrameState> entry;
            for (const auto& candidate : surfaces) {
                if (candidate->descriptor.expired()) {
                    entry = candidate;
                    break;
                }
            }
            if (!entry) {
                ++droppedFrames;
                ReportCapturePacing();
                return;
            }

            const std::uint64_t waitValue =
                entry->consumerFenceValue.load(std::memory_order_acquire);
            if (waitValue != 0 &&
                FAILED(d3d11Context4->Wait(consumerFence.Get(), waitValue))) {
                spdlog::error(
                    "[WebView2] Failed to enqueue the D3D11 consumer-fence "
                    "wait");
                return;
            }

            // WGC owns its frame-pool surface and may reuse it as soon as this
            // callback returns. Copy into our ring before checking the frame
            // back into the pool, then publish only after the producer signal
            // has been placed behind the copy.
            d3d11Context4->CopyResource(entry->texture.Get(), texture.Get());
            const std::uint64_t signalValue = ++producerFenceValue;
            result = d3d11Context4->Signal(producerFence.Get(), signalValue);
            if (FAILED(result)) {
                spdlog::error(
                    "[WebView2] Failed to signal captured frame {}: 0x{:08X}",
                    sequence + 1, static_cast<unsigned int>(result));
                return;
            }
            d3d11Context4->Flush();

            auto descriptor = MakeDescriptor(entry, signalValue);
            latestFrame.store(std::move(descriptor),
                              std::memory_order_release);
            ++capturedFrames;
            if (sequence == 1) {
                spdlog::info(
                    "[WebView2] First synchronized capture frame published");
            }
            ReportCapturePacing();
        } catch (const winrt::hresult_error& error) {
            spdlog::error("[WebView2] Frame capture failed: 0x{:08X}",
                          static_cast<unsigned int>(error.code()));
        }
    }

    bool CreateSharedSurfaceRing(const D3D11_TEXTURE2D_DESC& sourceDesc)
    {
        constexpr std::size_t surfaceCount = 3;
        latestFrame.store(nullptr, std::memory_order_release);
        surfaces.clear();
        ++generation;

        D3D11_TEXTURE2D_DESC desc = sourceDesc;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.SampleDesc = {1, 0};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE |
                         D3D11_BIND_RENDER_TARGET;
        desc.CPUAccessFlags = 0;
        // NT-handle D3D11 textures must also be created as shared resources.
        // Do not request KEYEDMUTEX here: this path transfers GPU ownership
        // with shared fences, and a keyed resource would additionally require
        // AcquireSync/ReleaseSync around every D3D11 access.
        desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_NTHANDLE |
                         D3D11_RESOURCE_MISC_SHARED;

        std::vector<std::shared_ptr<CapturedFrameState>> created;
        created.reserve(surfaceCount);
        for (std::size_t i = 0; i < surfaceCount; ++i) {
            auto entry = std::make_shared<CapturedFrameState>();
            HRESULT result = d3d11Device->CreateTexture2D(
                &desc, nullptr, &entry->texture);
            if (FAILED(result)) {
                spdlog::error(
                    "[WebView2] Could not allocate shared capture surface {}: "
                    "0x{:08X}",
                    i, static_cast<unsigned int>(result));
                return false;
            }

            ComPtr<IDXGIResource1> resource;
            result = entry->texture.As(&resource);
            if (SUCCEEDED(result)) {
                result = resource->CreateSharedHandle(
                    nullptr,
                    DXGI_SHARED_RESOURCE_READ |
                        DXGI_SHARED_RESOURCE_WRITE,
                    nullptr, &entry->sharedHandle);
            }
            if (FAILED(result)) {
                spdlog::error(
                    "[WebView2] Could not share capture surface {}: "
                    "0x{:08X}",
                    i, static_cast<unsigned int>(result));
                return false;
            }
            entry->width = desc.Width;
            entry->height = desc.Height;
            created.push_back(std::move(entry));
        }

        surfaceWidth = desc.Width;
        surfaceHeight = desc.Height;
        surfaceFormat = desc.Format;
        surfaces = std::move(created);
        spdlog::info(
            "[WebView2] Created {} synchronized shared surfaces at {}x{}",
            surfaceCount, surfaceWidth, surfaceHeight);
        return true;
    }

    std::shared_ptr<const CapturedFrame> MakeDescriptor(
        const std::shared_ptr<CapturedFrameState>& entry,
        std::uint64_t signalValue)
    {
        auto descriptor = std::shared_ptr<const CapturedFrame>(
            new CapturedFrame(
                entry->sharedHandle, producerFenceHandle,
                consumerFenceHandle, entry->width, entry->height,
                generation, ++sequence, signalValue, entry));
        entry->descriptor = descriptor;
        return descriptor;
    }

    void ReportCapturePacing()
    {
        const auto now = std::chrono::steady_clock::now();
        const double seconds =
            std::chrono::duration<double>(now - captureStatsStarted).count();
        if (seconds < 5.0) return;

        spdlog::debug(
            "[WebView2] Capture pacing: {:.1f} callbacks/s, {:.1f} "
            "published/s, {} ring drops",
            static_cast<double>(captureCallbacks) / seconds,
            static_cast<double>(capturedFrames) / seconds, droppedFrames);
        captureStatsStarted = now;
        captureCallbacks = 0;
        capturedFrames = 0;
        droppedFrames = 0;
    }

    void ApplyBounds()
    {
        if (!controller || desiredWidth == 0 || desiredHeight == 0) return;
        const RECT bounds{0, 0, static_cast<LONG>(desiredWidth),
                          static_cast<LONG>(desiredHeight)};
        controller->put_Bounds(bounds);
        if (rootVisual) {
            rootVisual.Size({static_cast<float>(desiredWidth),
                             static_cast<float>(desiredHeight)});
        }
    }

    void ApplyNavigation()
    {
        if (!webview || desiredUrl.empty()) return;
        const std::wstring url = ToWide(desiredUrl);
        if (!url.empty()) webview->Navigate(url.c_str());
    }

    void ApplyFocus()
    {
        if (!controller) return;
        if (desiredFocused) {
            controller->MoveFocus(
                COREWEBVIEW2_MOVE_FOCUS_REASON_PROGRAMMATIC);
        } else if (GetFocus() != parentWindow) {
            SetFocus(parentWindow);
        }
    }

    void FlushPendingJson()
    {
        if (!webview) return;
        for (const auto& json : pendingJson) {
            const std::wstring wide = ToWide(json);
            if (!wide.empty()) webview->PostWebMessageAsJson(wide.c_str());
        }
        pendingJson.clear();
    }

    void RecreateFramePool()
    {
        if (!framePool || desiredWidth == 0 || desiredHeight == 0) return;
        ++generation;
        surfaces.clear();
        surfaceWidth = 0;
        surfaceHeight = 0;
        surfaceFormat = DXGI_FORMAT_UNKNOWN;
        latestFrame.store(nullptr, std::memory_order_release);
        framePool.Recreate(
            direct3DDevice,
            winrt::Windows::Graphics::DirectX::
                DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2, {static_cast<int>(desiredWidth),
                static_cast<int>(desiredHeight)});
    }

    void SendMouse(const MouseCommand& mouse)
    {
        if (!compositionController) return;
        compositionController->SendMouseInput(
            mouse.kind, mouse.keys, mouse.data, mouse.point);
    }

    void CleanupOnThread()
    {
        ready.store(false, std::memory_order_release);
        latestFrame.store(nullptr, std::memory_order_release);
        surfaces.clear();

        if (framePool) framePool.FrameArrived(frameArrivedToken);
        if (captureSession) captureSession.Close();
        if (framePool) framePool.Close();
        captureSession = nullptr;
        framePool = nullptr;
        captureItem = nullptr;

        if (webview && webMessageToken.value)
            webview->remove_WebMessageReceived(webMessageToken);
        if (controller && acceleratorToken.value)
            controller->remove_AcceleratorKeyPressed(acceleratorToken);
        if (compositionController)
            compositionController->put_RootVisualTarget(nullptr);
        if (controller) controller->Close();

        webview.Reset();
        controller.Reset();
        compositionController.Reset();
        environment.Reset();
        environmentOptions.Reset();
        webviewVisual = nullptr;
        rootVisual = nullptr;
        compositor = nullptr;
        direct3DDevice = nullptr;
        consumerFence.Reset();
        producerFence.Reset();
        d3d11Context4.Reset();
        d3d11Device5.Reset();
        d3d11Context.Reset();
        d3d11Device.Reset();
        if (consumerFenceHandle) {
            CloseHandle(consumerFenceHandle);
            consumerFenceHandle = nullptr;
        }
        if (producerFenceHandle) {
            CloseHandle(producerFenceHandle);
            producerFenceHandle = nullptr;
        }
        if (dispatcherQueueController)
            dispatcherQueueController.ShutdownQueueAsync();
        dispatcherQueueController = nullptr;
    }

    mutable std::mutex lifecycleMutex;
    std::jthread thread;
    std::atomic_bool threadExited = true;
    HANDLE commandEvent = nullptr;
    std::atomic_bool running = false;
    std::atomic_bool ready = false;
    std::atomic_bool visible = false;
    std::atomic_bool focused = false;
    std::atomic_bool focusReleaseRequested = false;

    std::mutex commandMutex;
    std::deque<std::function<void()>> commands;

    std::mutex messageMutex;
    std::deque<std::string> messages;

    std::atomic<std::shared_ptr<const CapturedFrame>> latestFrame;

    HWND parentWindow = nullptr;
    LUID adapterLuid{};
    std::wstring userDataFolder;
    std::string desiredUrl;
    std::uint32_t desiredWidth = 0;
    std::uint32_t desiredHeight = 0;
    bool desiredFocused = false;
    std::vector<std::string> pendingJson;
    std::uint64_t generation = 1;
    std::uint64_t sequence = 0;
    std::uint64_t producerFenceValue = 0;
    std::uint64_t captureCallbacks = 0;
    std::uint64_t capturedFrames = 0;
    std::uint64_t droppedFrames = 0;
    std::chrono::steady_clock::time_point captureStatsStarted{};

    ComPtr<ID3D11Device> d3d11Device;
    ComPtr<ID3D11DeviceContext> d3d11Context;
    ComPtr<ID3D11Device5> d3d11Device5;
    ComPtr<ID3D11DeviceContext4> d3d11Context4;
    ComPtr<ID3D11Fence> producerFence;
    ComPtr<ID3D11Fence> consumerFence;
    HANDLE producerFenceHandle = nullptr;
    HANDLE consumerFenceHandle = nullptr;
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice
        direct3DDevice{nullptr};

    winrt::Windows::System::DispatcherQueueController
        dispatcherQueueController{nullptr};
    winrt::Windows::UI::Composition::Compositor compositor{nullptr};
    winrt::Windows::UI::Composition::ContainerVisual rootVisual{nullptr};
    winrt::Windows::UI::Composition::ContainerVisual webviewVisual{nullptr};
    winrt::Windows::Graphics::Capture::GraphicsCaptureItem
        captureItem{nullptr};
    winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool
        framePool{nullptr};
    winrt::Windows::Graphics::Capture::GraphicsCaptureSession
        captureSession{nullptr};
    winrt::event_token frameArrivedToken{};

    ComPtr<ICoreWebView2Environment> environment;
    ComPtr<ICoreWebView2EnvironmentOptions> environmentOptions;
    ComPtr<ICoreWebView2CompositionController> compositionController;
    ComPtr<ICoreWebView2Controller> controller;
    ComPtr<ICoreWebView2> webview;
    EventRegistrationToken webMessageToken{};
    EventRegistrationToken acceleratorToken{};

    std::vector<std::shared_ptr<CapturedFrameState>> surfaces;
    std::uint32_t surfaceWidth = 0;
    std::uint32_t surfaceHeight = 0;
    DXGI_FORMAT surfaceFormat = DXGI_FORMAT_UNKNOWN;
};

WebViewHost& WebViewHost::Instance()
{
    static WebViewHost instance;
    return instance;
}

WebViewHost::WebViewHost() : impl_(std::make_unique<Impl>()) {}
WebViewHost::~WebViewHost() = default;

bool WebViewHost::Start(HWND parentWindow, LUID adapterLuid,
                        std::wstring userDataFolder,
                        std::string initialUrl, std::uint32_t width,
                        std::uint32_t height)
{
    return impl_->Start(parentWindow, adapterLuid, std::move(userDataFolder),
                        std::move(initialUrl), width, height);
}

void WebViewHost::Stop() { impl_->Stop(); }

bool WebViewHost::IsRunning() const
{
    return impl_->running.load(std::memory_order_acquire);
}

bool WebViewHost::IsReady() const
{
    return impl_->ready.load(std::memory_order_acquire);
}

bool WebViewHost::IsVisible() const
{
    return impl_->visible.load(std::memory_order_acquire);
}

bool WebViewHost::IsFocused() const
{
    return impl_->focused.load(std::memory_order_acquire);
}

bool WebViewHost::TakeFocusReleaseRequest()
{
    return impl_->focusReleaseRequested.exchange(
        false, std::memory_order_acq_rel);
}

void WebViewHost::Navigate(std::string url)
{
    impl_->Post([impl = impl_.get(), url = std::move(url)]() mutable {
        impl->desiredUrl = std::move(url);
        impl->ApplyNavigation();
    });
}

void WebViewHost::Resize(std::uint32_t width, std::uint32_t height)
{
    if (width == 0 || height == 0) return;
    impl_->Post([impl = impl_.get(), width, height] {
        if (impl->desiredWidth == width && impl->desiredHeight == height)
            return;
        impl->desiredWidth = width;
        impl->desiredHeight = height;
        impl->ApplyBounds();
        impl->RecreateFramePool();
    });
}

void WebViewHost::SetVisible(bool visible)
{
    impl_->visible.store(visible, std::memory_order_release);
}

void WebViewHost::SetFocused(bool focused)
{
    impl_->focused.store(focused, std::memory_order_release);
    impl_->Post([impl = impl_.get(), focused] {
        impl->desiredFocused = focused;
        impl->ApplyFocus();
    });
}

void WebViewHost::PostJson(std::string json)
{
    impl_->Post([impl = impl_.get(), json = std::move(json)]() mutable {
        if (!impl->webview) {
            impl->pendingJson.push_back(std::move(json));
            return;
        }
        const std::wstring wide = ToWide(json);
        if (!wide.empty()) impl->webview->PostWebMessageAsJson(wide.c_str());
    });
}

bool WebViewHost::HandleWindowMessage(HWND window, UINT message,
                                      WPARAM wParam, LPARAM lParam)
{
    if (!impl_->ready.load(std::memory_order_acquire) ||
        !impl_->focused.load(std::memory_order_acquire)) {
        return false;
    }

    MouseCommand mouse{};
    bool isMouse = true;
    switch (message) {
        case WM_MOUSEMOVE:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_MOVE;
            {
                TRACKMOUSEEVENT tracking{sizeof(TRACKMOUSEEVENT), TME_LEAVE,
                                         window, 0};
                TrackMouseEvent(&tracking);
            }
            break;
        case WM_MOUSELEAVE:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_LEAVE;
            break;
        case WM_LBUTTONDOWN:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_LEFT_BUTTON_DOWN;
            break;
        case WM_LBUTTONUP:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_LEFT_BUTTON_UP;
            break;
        case WM_LBUTTONDBLCLK:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_LEFT_BUTTON_DOUBLE_CLICK;
            break;
        case WM_RBUTTONDOWN:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_RIGHT_BUTTON_DOWN;
            break;
        case WM_RBUTTONUP:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_RIGHT_BUTTON_UP;
            break;
        case WM_RBUTTONDBLCLK:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_RIGHT_BUTTON_DOUBLE_CLICK;
            break;
        case WM_MBUTTONDOWN:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_MIDDLE_BUTTON_DOWN;
            break;
        case WM_MBUTTONUP:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_MIDDLE_BUTTON_UP;
            break;
        case WM_MBUTTONDBLCLK:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_MIDDLE_BUTTON_DOUBLE_CLICK;
            break;
        case WM_XBUTTONDOWN:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_X_BUTTON_DOWN;
            mouse.data = GET_XBUTTON_WPARAM(wParam);
            break;
        case WM_XBUTTONUP:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_X_BUTTON_UP;
            mouse.data = GET_XBUTTON_WPARAM(wParam);
            break;
        case WM_XBUTTONDBLCLK:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_X_BUTTON_DOUBLE_CLICK;
            mouse.data = GET_XBUTTON_WPARAM(wParam);
            break;
        case WM_MOUSEWHEEL:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_WHEEL;
            mouse.data = static_cast<std::uint32_t>(
                static_cast<std::int32_t>(GET_WHEEL_DELTA_WPARAM(wParam)));
            break;
        case WM_MOUSEHWHEEL:
            mouse.kind = COREWEBVIEW2_MOUSE_EVENT_KIND_HORIZONTAL_WHEEL;
            mouse.data = static_cast<std::uint32_t>(
                static_cast<std::int32_t>(GET_WHEEL_DELTA_WPARAM(wParam)));
            break;
        default:
            isMouse = false;
            break;
    }

    if (isMouse) {
        if (message != WM_MOUSELEAVE) {
            mouse.point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (message == WM_MOUSEWHEEL || message == WM_MOUSEHWHEEL)
                ScreenToClient(window, &mouse.point);
            mouse.keys = MouseVirtualKeys(wParam);
        }
        impl_->Post(
            [impl = impl_.get(), mouse] { impl->SendMouse(mouse); });
        return true;
    }

    // MoveFocus transfers keyboard input to WebView2's controller HWND. The
    // game-window subclass should not synthesize or swallow keyboard messages.
    return false;
}

bool WebViewHost::PollMessage(std::string& json)
{
    std::scoped_lock lock(impl_->messageMutex);
    if (impl_->messages.empty()) return false;
    json = std::move(impl_->messages.front());
    impl_->messages.pop_front();
    return true;
}

std::shared_ptr<const CapturedFrame> WebViewHost::LatestFrame() const
{
    return impl_->latestFrame.load(std::memory_order_acquire);
}

} // namespace rdr2::webview
