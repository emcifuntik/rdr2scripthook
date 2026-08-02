#pragma once

#include <windows.h>

#include <cstdint>
#include <memory>
#include <string>

namespace rdr2::webview {

struct CapturedFrameState;

class CapturedFrame final {
public:
    HANDLE SharedHandle() const { return sharedHandle_; }
    HANDLE ProducerFenceHandle() const { return producerFenceHandle_; }
    HANDLE ConsumerFenceHandle() const { return consumerFenceHandle_; }
    std::uint32_t Width() const { return width_; }
    std::uint32_t Height() const { return height_; }
    std::uint64_t Generation() const { return generation_; }
    std::uint64_t Sequence() const { return sequence_; }
    std::uint64_t ProducerFenceValue() const {
        return producerFenceValue_;
    }

    // Called by the render thread after the RAGE command list containing this
    // frame has been submitted. The capture device waits for this value before
    // reusing the backing texture.
    void MarkSubmitted(std::uint64_t consumerFenceValue) const;

private:
    friend class WebViewHost;

    CapturedFrame(HANDLE sharedHandle, HANDLE producerFenceHandle,
                  HANDLE consumerFenceHandle, std::uint32_t width,
                  std::uint32_t height, std::uint64_t generation,
                  std::uint64_t sequence,
                  std::uint64_t producerFenceValue,
                  std::shared_ptr<CapturedFrameState> state);

    HANDLE sharedHandle_ = nullptr;
    HANDLE producerFenceHandle_ = nullptr;
    HANDLE consumerFenceHandle_ = nullptr;
    std::uint32_t width_ = 0;
    std::uint32_t height_ = 0;
    std::uint64_t generation_ = 0;
    std::uint64_t sequence_ = 0;
    std::uint64_t producerFenceValue_ = 0;
    std::shared_ptr<CapturedFrameState> state_;
};

class WebViewHost final {
public:
    static WebViewHost& Instance();

    // Starts a composition WebView on a dedicated STA thread. adapterLuid must
    // identify the adapter used by RDR2's D3D12 device.
    bool Start(HWND parentWindow, LUID adapterLuid,
               std::wstring userDataFolder, std::string initialUrl,
               std::uint32_t width, std::uint32_t height);
    void Stop();

    bool IsRunning() const;
    bool IsReady() const;
    bool IsVisible() const;
    bool IsFocused() const;
    bool TakeFocusReleaseRequest();

    void Navigate(std::string url);
    void Resize(std::uint32_t width, std::uint32_t height);
    void SetVisible(bool visible);
    void SetFocused(bool focused);
    void PostJson(std::string json);

    // Returns true when a focused WebView consumed the window message.
    bool HandleWindowMessage(HWND window, UINT message, WPARAM wParam,
                             LPARAM lParam);

    bool PollMessage(std::string& json);
    std::shared_ptr<const CapturedFrame> LatestFrame() const;

private:
    WebViewHost();
    ~WebViewHost();
    WebViewHost(const WebViewHost&) = delete;
    WebViewHost& operator=(const WebViewHost&) = delete;

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace rdr2::webview
