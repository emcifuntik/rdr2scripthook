#include "stdafx.h"

#include "WebViewOverlay.h"

#include "WebViewHost.h"
#include "graphics/PostFrontendRenderer.h"
#include "graphics/RageUiRenderer.h"
#include "input/GameInputHook.h"

#include <filesystem>
#include <system_error>

namespace rdr2::webview {
namespace {

HWND FindGameWindow()
{
    HWND window = FindWindowA("sgaWindow", "Red Dead Redemption 2");
    if (!window) window = FindWindowA("sgaWindow", nullptr);
    return window;
}

std::wstring UserDataFolder()
{
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&UserDataFolder), &module)) {
        return {};
    }

    std::wstring path(32768, L'\0');
    const DWORD length = GetModuleFileNameW(
        module, path.data(), static_cast<DWORD>(path.size()));
    if (length == 0 || length >= path.size()) return {};
    path.resize(length);

    std::filesystem::path folder =
        std::filesystem::path(path).parent_path() / L"webview2";
    std::error_code error;
    std::filesystem::create_directories(folder, error);
    if (error) {
        spdlog::error("[WebView2] Could not create user-data directory: {}",
                      error.message());
        return {};
    }
    return folder.wstring();
}

} // namespace

WebViewOverlay& WebViewOverlay::Instance()
{
    static WebViewOverlay instance;
    return instance;
}

OverlayStatus WebViewOverlay::Open(std::string url, std::uint32_t width,
                                   std::uint32_t height)
{
    if (url.empty()) url = "about:blank";
    if (!graphics::RageUiRenderer::IsReady() ||
        !graphics::IsPostFrontendReady())
        return OverlayStatus::RendererUnavailable;

    HWND window = FindGameWindow();
    if (!window) return OverlayStatus::GameWindowUnavailable;

    if (width == 0 || height == 0) {
        RECT client{};
        if (!GetClientRect(window, &client))
            return OverlayStatus::InvalidArgument;
        width = static_cast<std::uint32_t>(client.right - client.left);
        height = static_cast<std::uint32_t>(client.bottom - client.top);
    }
    if (width == 0 || height == 0)
        return OverlayStatus::InvalidArgument;

    auto& host = WebViewHost::Instance();
    if (host.IsRunning()) {
        host.Resize(width, height);
        host.Navigate(std::move(url));
        host.SetVisible(true);
        return OverlayStatus::Ok;
    }

    LUID adapterLuid{};
    const auto backend = graphics::GetGraphicsBackend();
    if (backend == graphics::GraphicsBackend::Unknown ||
        !graphics::GetGraphicsAdapterLuid(adapterLuid)) {
        return OverlayStatus::GraphicsDeviceUnavailable;
    }
    if (!host.Start(window, adapterLuid, UserDataFolder(),
                    std::move(url), width, height)) {
        return OverlayStatus::StartFailed;
    }

    graphics::SetPostFrontendCallback(&WebViewOverlay::Render);
    graphics::SetPostFrontendSubmittedCallback(
        &WebViewOverlay::RenderSubmitted);
    spdlog::info("[WebView2] Overlay requested at {}x{} using {}", width,
                 height,
                 backend == graphics::GraphicsBackend::Vulkan ? "Vulkan"
                                                               : "D3D12");
    return OverlayStatus::Ok;
}

void WebViewOverlay::Close()
{
    auto& host = WebViewHost::Instance();
    host.SetVisible(false);
    host.SetFocused(false);
    host.Stop();
    input::GameInputHook::SetBlocked(false);
    // Render() releases the imported RAGE texture on the render thread and
    // then removes the callback.
}

bool WebViewOverlay::IsOpen() const
{
    return WebViewHost::Instance().IsRunning();
}

void WebViewOverlay::Navigate(std::string url)
{
    if (!url.empty()) WebViewHost::Instance().Navigate(std::move(url));
}

void WebViewOverlay::SetVisible(bool visible)
{
    WebViewHost::Instance().SetVisible(visible);
}

bool WebViewOverlay::IsVisible() const
{
    return WebViewHost::Instance().IsVisible();
}

void WebViewOverlay::SetFocused(bool focused)
{
    input::GameInputHook::SetBlocked(focused);
    WebViewHost::Instance().SetFocused(focused);
}

bool WebViewOverlay::IsFocused() const
{
    return WebViewHost::Instance().IsFocused();
}

void WebViewOverlay::PostJson(std::string json)
{
    WebViewHost::Instance().PostJson(std::move(json));
}

bool WebViewOverlay::PollMessage(std::string& json)
{
    return WebViewHost::Instance().PollMessage(json);
}

bool WebViewOverlay::Pump()
{
    auto& host = WebViewHost::Instance();
    const bool releaseFocus = host.TakeFocusReleaseRequest();
    if (releaseFocus) SetFocused(false);
    if (!host.IsRunning() && input::GameInputHook::IsBlocked())
        input::GameInputHook::SetBlocked(false);
    return releaseFocus;
}

bool WebViewOverlay::HandleWindowMessage(HWND window, UINT message,
                                         WPARAM wParam, LPARAM lParam)
{
    auto& host = WebViewHost::Instance();
    if (message == WM_INPUT && host.IsFocused()) {
        DefWindowProcW(window, message, wParam, lParam);
        return true;
    }
    if (message == WM_SIZE && wParam != SIZE_MINIMIZED && host.IsRunning()) {
        const auto width = static_cast<std::uint32_t>(LOWORD(lParam));
        const auto height = static_cast<std::uint32_t>(HIWORD(lParam));
        if (width && height) host.Resize(width, height);
    }
    return host.HandleWindowMessage(window, message, wParam, lParam);
}

void WebViewOverlay::Render(void* graphicsContext)
{
    auto& host = WebViewHost::Instance();
    auto frame = host.LatestFrame();
    if (host.IsVisible())
        graphics::RageUiRenderer::Render(graphicsContext, frame);

    if (!host.IsRunning() && !frame) {
        host.Stop();
        graphics::RageUiRenderer::Reset();
        graphics::SetPostFrontendSubmittedCallback(nullptr);
        graphics::SetPostFrontendCallback(nullptr);
    }
}

void WebViewOverlay::RenderSubmitted()
{
    graphics::RageUiRenderer::OnSubmitted();
}

} // namespace rdr2::webview
