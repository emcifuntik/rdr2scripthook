#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <d3d11_4.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <cstdio>

using Microsoft::WRL::ComPtr;

static void Probe(ID3D11Device* device11, ID3D12Device* device12,
                  const char* name, UINT miscFlags)
{
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = 64;
    desc.Height = 64;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.MiscFlags = miscFlags;

    ComPtr<ID3D11Texture2D> texture11;
    HRESULT createResult =
        device11->CreateTexture2D(&desc, nullptr, &texture11);
    std::printf("%-28s CreateTexture2D=0x%08lX", name,
                static_cast<unsigned long>(createResult));
    if (FAILED(createResult)) {
        std::puts("");
        return;
    }

    ComPtr<IDXGIResource1> dxgiResource;
    HRESULT queryResult = texture11.As(&dxgiResource);
    HANDLE handle = nullptr;
    HRESULT handleResult = SUCCEEDED(queryResult)
                               ? dxgiResource->CreateSharedHandle(
                                     nullptr,
                                     DXGI_SHARED_RESOURCE_READ |
                                         DXGI_SHARED_RESOURCE_WRITE,
                                     nullptr, &handle)
                               : queryResult;
    std::printf(" CreateSharedHandle=0x%08lX",
                static_cast<unsigned long>(handleResult));

    ComPtr<ID3D12Resource> texture12;
    HRESULT openResult = SUCCEEDED(handleResult)
                             ? device12->OpenSharedHandle(
                                   handle, IID_PPV_ARGS(&texture12))
                             : handleResult;
    std::printf(" OpenSharedHandle=0x%08lX\n",
                static_cast<unsigned long>(openResult));
    if (handle) CloseHandle(handle);
}

int main()
{
    ComPtr<IDXGIFactory4> factory;
    HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(result)) return 1;

    ComPtr<IDXGIAdapter1> adapter;
    result = factory->EnumAdapters1(0, &adapter);
    if (FAILED(result)) return 2;

    ComPtr<ID3D11Device> device11;
    ComPtr<ID3D11DeviceContext> context11;
    result = D3D11CreateDevice(
        adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
        &device11, nullptr, &context11);
    if (FAILED(result)) return 3;

    ComPtr<ID3D12Device> device12;
    result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                               IID_PPV_ARGS(&device12));
    if (FAILED(result)) return 4;

    Probe(device11.Get(), device12.Get(), "NTHANDLE",
          D3D11_RESOURCE_MISC_SHARED_NTHANDLE);
    Probe(device11.Get(), device12.Get(), "SHARED | NTHANDLE",
          D3D11_RESOURCE_MISC_SHARED |
              D3D11_RESOURCE_MISC_SHARED_NTHANDLE);
    Probe(device11.Get(), device12.Get(), "KEYEDMUTEX | NTHANDLE",
          D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX |
              D3D11_RESOURCE_MISC_SHARED_NTHANDLE);
    return 0;
}
