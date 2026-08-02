#pragma once

#include <memory>

namespace rdr2::webview {
class CapturedFrame;
}

namespace rdr2::graphics {

// Draws WebView2's latest composition surface through RAGE's immediate-mode
// UI path. All methods except IsReady must be called from the render thread.
class RageUiRenderer final {
public:
    static bool IsReady();
    static void Render(
        void* graphicsContext,
        const std::shared_ptr<const webview::CapturedFrame>& frame);
    static void OnSubmitted();
    static void Reset();
};

} // namespace rdr2::graphics
