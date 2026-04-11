#include "includes.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);

extern "C" __declspec(dllexport) HRESULT __stdcall proxyDirectInput8Create(
    HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut,
    LPUNKNOWN punkOuter) {
  using tDirectInput8Create =
      HRESULT(__stdcall*)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

  static HMODULE realDinput8 = nullptr;
  static tDirectInput8Create pDirectInput8Create = nullptr;

  if (!realDinput8) {
    char sysPath[MAX_PATH];
    GetSystemDirectoryA(sysPath, MAX_PATH);
    strcat_s(sysPath, "\\dinput8.dll");

    realDinput8 = LoadLibraryA(sysPath);
  }

  if (!realDinput8) return DIERR_GENERIC;

  if (!pDirectInput8Create) {
    pDirectInput8Create = reinterpret_cast<tDirectInput8Create>(
        GetProcAddress(realDinput8, "DirectInput8Create"));
  }

  if (!pDirectInput8Create) return DIERR_GENERIC;

  return pDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui() {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplWin32_Init(window);
  ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam,
                          LPARAM lParam) {
  if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    return true;

  return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval,
                            UINT Flags) {
  if (!init) {
    if (SUCCEEDED(
            pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
      pDevice->GetImmediateContext(&pContext);
      DXGI_SWAP_CHAIN_DESC sd;
      pSwapChain->GetDesc(&sd);
      window = sd.OutputWindow;
      ID3D11Texture2D* pBackBuffer;
      pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                            (LPVOID*)&pBackBuffer);
      pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
      pBackBuffer->Release();
      oWndProc =
          (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
      InitImGui();
      init = true;
    }

    else
      return oPresent(pSwapChain, SyncInterval, Flags);
  }

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("ImGui Window");
  ImGui::End();

  ImGui::Render();

  pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved) {
  bool init_hook = false;
  do {
    if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
      kiero::bind(8, (void**)&oPresent, hkPresent);
      init_hook = true;
    }
  } while (!init_hook);
  return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
  switch (dwReason) {
    case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls(hMod);
      CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
      break;
    case DLL_PROCESS_DETACH:
      kiero::shutdown();
      break;
  }
  return TRUE;
}
