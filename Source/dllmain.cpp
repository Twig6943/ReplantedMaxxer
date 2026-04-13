#include "includes.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);

// dinput8
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

// Globals
Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

bool init = false;
bool showMenu = true;

// ImGui Init
void InitImGui() {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

  // Style
  ImGui::StyleColorsDark();
  float dpiScale = 1.0f;  // adjust if you actually calculate DPI

  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(dpiScale);
  style.FontScaleDpi = dpiScale;

  style.FrameBorderSize = 1.0f;
  style.FrameRounding = 2.0f;
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

  style.Colors[ImGuiCol_Button] = ImColor(50, 200, 90, 255).Value;
  style.Colors[ImGuiCol_ButtonHovered] = ImColor(70, 240, 120, 230).Value;
  style.Colors[ImGuiCol_ButtonActive] = ImColor(40, 170, 80, 255).Value;

  style.Colors[ImGuiCol_CheckMark] = ImColor(150, 255, 160, 255).Value;

  style.Colors[ImGuiCol_FrameBg] = ImColor(25, 35, 25, 160).Value;
  style.Colors[ImGuiCol_FrameBgActive] = ImColor(60, 100, 60, 200).Value;
  style.Colors[ImGuiCol_FrameBgHovered] = ImColor(45, 75, 45, 140).Value;

  style.Colors[ImGuiCol_ResizeGrip] = ImColor(90, 230, 130, 150).Value;

  style.Colors[ImGuiCol_SliderGrab] = ImColor(80, 220, 120, 255).Value;
  style.Colors[ImGuiCol_SliderGrabActive] = ImColor(110, 255, 150, 255).Value;

  style.Colors[ImGuiCol_Tab] = ImColor(60, 180, 100, 230).Value;
  style.Colors[ImGuiCol_TabHovered] = ImColor(80, 240, 130, 230).Value;
  style.Colors[ImGuiCol_TabSelected] = ImColor(70, 200, 110, 255).Value;

  style.Colors[ImGuiCol_TitleBgActive] = ImColor(60, 180, 100, 255).Value;

  style.Colors[ImGuiCol_WindowBg] = ImColor(15, 25, 15, 255).Value;

  ImGui_ImplWin32_Init(window);
  ImGui_ImplDX11_Init(pDevice, pContext);
}

// WndProc Hook
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam,
                          LPARAM lParam) {
  // Toggle menu with F1 (single press)
  if (uMsg == WM_KEYDOWN && wParam == VK_F1) {
    showMenu = !showMenu;
    return 0;  // block further processing (optional)
  }

  // Only send input to ImGui when menu is visible
  if (showMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    return true;

  return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

// Present Hook
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
    } else {
      return oPresent(pSwapChain, SyncInterval, Flags);
    }
  }

  // Start frame
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // Draw only if toggled on
  if (showMenu) {
    ImGui::Begin("ImGui Window");
    ImGui::Text("(Press F1 to toggle)");
    ImGui::End();
  }

  // Optional: show cursor only when menu is open
  ImGuiIO& io = ImGui::GetIO();
  io.MouseDrawCursor = showMenu;

  // Render
  ImGui::Render();
  pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  return oPresent(pSwapChain, SyncInterval, Flags);
}

// Thread / Hook Init
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

// DLL Entry
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
