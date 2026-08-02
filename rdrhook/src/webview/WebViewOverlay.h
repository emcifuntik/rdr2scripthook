#pragma once

#include <windows.h>

#include <cstdint>
#include <string>

namespace rdr2::webview {

enum class OverlayStatus : std::int32_t {
    Ok = 0,
    InvalidArgument = -1,
    RendererUnavailable = -2,
    GameWindowUnavailable = -3,
    GraphicsDeviceUnavailable = -4,
    StartFailed = -5,
};

// Owns the first end-to-end WebView overlay. The scripting ABI deliberately
// exposes this as a singleton while the render and input contracts stabilize;
// it can later become a handle table without changing WebViewHost itself.
class WebViewOverlay final {
public:
    static WebViewOverlay& Instance();

    OverlayStatus Open(std::string url, std::uint32_t width,
                       std::uint32_t height);
    void Close();
    bool IsOpen() const;

    void Navigate(std::string url);
    void SetVisible(bool visible);
    bool IsVisible() const;
    void SetFocused(bool focused);
    bool IsFocused() const;
    void PostJson(std::string json);
    bool PollMessage(std::string& json);

    // Called from the game script thread. Returns true when WebView2 consumed
    // the reserved F8 accelerator and released focus.
    bool Pump();

    bool HandleWindowMessage(HWND window, UINT message, WPARAM wParam,
                             LPARAM lParam);

private:
    static void Render(void* graphicsContext);
    static void RenderSubmitted();
};

} // namespace rdr2::webview
