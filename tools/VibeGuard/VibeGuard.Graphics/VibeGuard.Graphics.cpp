#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <atomic>
#include <new>

extern "C" IMAGE_DOS_HEADER __ImageBase;

namespace
{
    using Direct3DCreate9ExFunction = HRESULT (WINAPI*)(UINT, IDirect3D9Ex**);

    Direct3DCreate9ExFunction LoadSystemDirect3DCreate9Ex()
    {
        wchar_t systemDirectory[MAX_PATH]{};
        const UINT length = GetSystemDirectoryW(systemDirectory, static_cast<UINT>(_countof(systemDirectory)));
        if (length == 0 || length >= _countof(systemDirectory))
            return nullptr;
        if (wcscat_s(systemDirectory, L"\\d3d9.dll") != 0)
            return nullptr;
        HMODULE module = LoadLibraryW(systemDirectory);
        if (module == nullptr)
            return nullptr;
        return reinterpret_cast<Direct3DCreate9ExFunction>(GetProcAddress(module, "Direct3DCreate9Ex"));
    }

    bool ShouldUseVSync()
    {
        wchar_t modulePath[MAX_PATH]{};
        if (GetModuleFileNameW(
                reinterpret_cast<HMODULE>(&__ImageBase),
                modulePath,
                static_cast<DWORD>(_countof(modulePath))) == 0)
            return true;

        wchar_t* separator = wcsrchr(modulePath, L'\\');
        if (separator == nullptr)
            return true;
        wcscpy_s(separator + 1, _countof(modulePath) - (separator + 1 - modulePath), L"vibeguard-graphics.ini");
        return GetPrivateProfileIntW(L"VibeGuard.Graphics", L"UseVSync", 1, modulePath) != 0;
    }

    class Direct3D9ExWrapper final : public IDirect3D9Ex
    {
    public:
        explicit Direct3D9ExWrapper(IDirect3D9Ex* inner) : inner_(inner) {}

        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override
        {
            if (object == nullptr)
                return E_POINTER;
            if (riid == IID_IUnknown || riid == IID_IDirect3D9 || riid == IID_IDirect3D9Ex)
            {
                *object = this;
                AddRef();
                return S_OK;
            }
            return inner_->QueryInterface(riid, object);
        }

        ULONG STDMETHODCALLTYPE AddRef() override
        {
            references_.fetch_add(1, std::memory_order_relaxed);
            return inner_->AddRef();
        }

        ULONG STDMETHODCALLTYPE Release() override
        {
            const ULONG result = inner_->Release();
            if (references_.fetch_sub(1, std::memory_order_acq_rel) == 1)
                delete this;
            return result;
        }

        HRESULT STDMETHODCALLTYPE RegisterSoftwareDevice(void* initializeFunction) override { return inner_->RegisterSoftwareDevice(initializeFunction); }
        UINT STDMETHODCALLTYPE GetAdapterCount() override { return inner_->GetAdapterCount(); }
        HRESULT STDMETHODCALLTYPE GetAdapterIdentifier(UINT adapter, DWORD flags, D3DADAPTER_IDENTIFIER9* identifier) override { return inner_->GetAdapterIdentifier(adapter, flags, identifier); }
        UINT STDMETHODCALLTYPE GetAdapterModeCount(UINT adapter, D3DFORMAT format) override { return inner_->GetAdapterModeCount(adapter, format); }
        HRESULT STDMETHODCALLTYPE EnumAdapterModes(UINT adapter, D3DFORMAT format, UINT mode, D3DDISPLAYMODE* displayMode) override { return inner_->EnumAdapterModes(adapter, format, mode, displayMode); }
        HRESULT STDMETHODCALLTYPE GetAdapterDisplayMode(UINT adapter, D3DDISPLAYMODE* mode) override { return inner_->GetAdapterDisplayMode(adapter, mode); }
        HRESULT STDMETHODCALLTYPE CheckDeviceType(UINT adapter, D3DDEVTYPE deviceType, D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat, BOOL windowed) override { return inner_->CheckDeviceType(adapter, deviceType, adapterFormat, backBufferFormat, windowed); }
        HRESULT STDMETHODCALLTYPE CheckDeviceFormat(UINT adapter, D3DDEVTYPE deviceType, D3DFORMAT adapterFormat, DWORD usage, D3DRESOURCETYPE resourceType, D3DFORMAT checkFormat) override { return inner_->CheckDeviceFormat(adapter, deviceType, adapterFormat, usage, resourceType, checkFormat); }
        HRESULT STDMETHODCALLTYPE CheckDeviceMultiSampleType(UINT adapter, D3DDEVTYPE deviceType, D3DFORMAT surfaceFormat, BOOL windowed, D3DMULTISAMPLE_TYPE multiSampleType, DWORD* qualityLevels) override { return inner_->CheckDeviceMultiSampleType(adapter, deviceType, surfaceFormat, windowed, multiSampleType, qualityLevels); }
        HRESULT STDMETHODCALLTYPE CheckDepthStencilMatch(UINT adapter, D3DDEVTYPE deviceType, D3DFORMAT adapterFormat, D3DFORMAT renderTargetFormat, D3DFORMAT depthStencilFormat) override { return inner_->CheckDepthStencilMatch(adapter, deviceType, adapterFormat, renderTargetFormat, depthStencilFormat); }
        HRESULT STDMETHODCALLTYPE CheckDeviceFormatConversion(UINT adapter, D3DDEVTYPE deviceType, D3DFORMAT sourceFormat, D3DFORMAT targetFormat) override { return inner_->CheckDeviceFormatConversion(adapter, deviceType, sourceFormat, targetFormat); }
        HRESULT STDMETHODCALLTYPE GetDeviceCaps(UINT adapter, D3DDEVTYPE deviceType, D3DCAPS9* caps) override { return inner_->GetDeviceCaps(adapter, deviceType, caps); }
        HMONITOR STDMETHODCALLTYPE GetAdapterMonitor(UINT adapter) override { return inner_->GetAdapterMonitor(adapter); }
        HRESULT STDMETHODCALLTYPE CreateDevice(UINT adapter, D3DDEVTYPE deviceType, HWND focusWindow, DWORD behaviorFlags, D3DPRESENT_PARAMETERS* parameters, IDirect3DDevice9** device) override
        {
            if (parameters != nullptr && !ShouldUseVSync())
                parameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            return inner_->CreateDevice(adapter, deviceType, focusWindow, behaviorFlags, parameters, device);
        }
        UINT STDMETHODCALLTYPE GetAdapterModeCountEx(UINT adapter, const D3DDISPLAYMODEFILTER* filter) override { return inner_->GetAdapterModeCountEx(adapter, filter); }
        HRESULT STDMETHODCALLTYPE EnumAdapterModesEx(UINT adapter, const D3DDISPLAYMODEFILTER* filter, UINT mode, D3DDISPLAYMODEEX* displayMode) override { return inner_->EnumAdapterModesEx(adapter, filter, mode, displayMode); }
        HRESULT STDMETHODCALLTYPE GetAdapterDisplayModeEx(UINT adapter, D3DDISPLAYMODEEX* mode, D3DDISPLAYROTATION* rotation) override { return inner_->GetAdapterDisplayModeEx(adapter, mode, rotation); }
        HRESULT STDMETHODCALLTYPE CreateDeviceEx(UINT adapter, D3DDEVTYPE deviceType, HWND focusWindow, DWORD behaviorFlags, D3DPRESENT_PARAMETERS* parameters, D3DDISPLAYMODEEX* fullscreenMode, IDirect3DDevice9Ex** device) override
        {
            if (parameters != nullptr && !ShouldUseVSync())
                parameters->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            return inner_->CreateDeviceEx(adapter, deviceType, focusWindow, behaviorFlags, parameters, fullscreenMode, device);
        }
        HRESULT STDMETHODCALLTYPE GetAdapterLUID(UINT adapter, LUID* luid) override { return inner_->GetAdapterLUID(adapter, luid); }

    private:
        std::atomic<ULONG> references_{1};
        IDirect3D9Ex* inner_;
    };

}

extern "C" HRESULT WINAPI Direct3DCreate9Ex(UINT sdkVersion, IDirect3D9Ex** direct3D)
{
    if (direct3D == nullptr)
        return E_POINTER;
    static const Direct3DCreate9ExFunction systemDirect3DCreate9Ex = LoadSystemDirect3DCreate9Ex();
    if (systemDirect3DCreate9Ex == nullptr)
        return D3DERR_NOTAVAILABLE;

    IDirect3D9Ex* inner = nullptr;
    const HRESULT result = systemDirect3DCreate9Ex(sdkVersion, &inner);
    if (FAILED(result) || inner == nullptr)
        return result;
    auto* wrapper = new (std::nothrow) Direct3D9ExWrapper(inner);
    if (wrapper == nullptr)
    {
        inner->Release();
        return E_OUTOFMEMORY;
    }
    *direct3D = wrapper;
    return result;
}

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
        DisableThreadLibraryCalls(reinterpret_cast<HMODULE>(&__ImageBase));
    return TRUE;
}
