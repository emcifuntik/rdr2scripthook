#pragma once

#include <Windows.h>

#include <cstdint>

struct ID3D12Device;
struct ID3D12CommandQueue;

namespace rdr2::graphics {

enum class GraphicsBackend {
    Unknown,
    D3D12,
    Vulkan,
};

using PostFrontendCallback = void (*)(void* graphicsContext);
using PostFrontendSubmittedCallback = void (*)();

// The callback is invoked on RDR2's render thread after the game frontend/HUD
// has been composed and the swap-chain backbuffer has been rebound.
void SetPostFrontendCallback(PostFrontendCallback callback);

// Invoked immediately after the post-frontend draw is handed to the active
// RDR3 graphics queue. This is the safe point for signaling consumer fences.
void SetPostFrontendSubmittedCallback(
    PostFrontendSubmittedCallback callback);

// Returns true once the RDR3 end-draw hook and all state-restoration helpers
// needed by the post-frontend layer have been resolved.
bool IsPostFrontendReady();

// Returns the D3D12 device selected by RDR2, or nullptr until SGA has created
// it (and when the game is running with another graphics backend).
ID3D12Device* GetD3D12Device();

// Returns RDR3's direct graphics queue. The pointer is owned by the game.
ID3D12CommandQueue* GetD3D12CommandQueue();

// Returns the active RDR3 graphics backend. The result stays Unknown until
// the corresponding device and direct graphics queue have been created.
GraphicsBackend GetGraphicsBackend();

// Vulkan handles are kept opaque here so consumers which do not use Vulkan
// do not have to include the Vulkan headers.
void* GetVulkanDevice();
void* GetVulkanQueue();

// Returns the DXGI adapter used for the WebView2 capture device. D3D12 can
// provide the exact LUID; Vulkan uses the high-performance hardware adapter
// selected by the same Windows GPU preference RDR2 normally follows.
bool GetGraphicsAdapterLuid(LUID& adapterLuid);

// Stages Direct3D-fence-backed Vulkan semaphores around RAGE's next graphics
// queue submission. The actual vkQueueSubmit calls run on RAGE's submission
// thread, preserving Vulkan's external queue-synchronization requirement.
bool StageVulkanProducerWait(void* semaphore, std::uint64_t value);
bool StageVulkanConsumerSignal(void* semaphore, std::uint64_t value);

} // namespace rdr2::graphics
