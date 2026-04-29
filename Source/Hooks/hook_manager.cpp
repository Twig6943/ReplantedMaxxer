#include "hook_manager.h"
#include "../Renderer/renderer.h"
#include "../UI/menu.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK HookManager::WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  auto& menu = Menu::GetInstance();
  
  if (uMsg == WM_KEYDOWN && wParam == VK_F1) {
    menu.Toggle();
    return 0;
  }

  if (menu.IsVisible() && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    return true;

  return CallWindowProc(GetInstance().GetOriginalWndProc(), hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall HookManager::PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
  auto& renderer = Renderer::GetInstance();
  auto& menu = Menu::GetInstance();

  if (!renderer.IsInitialized()) {
    if (renderer.Initialize(pSwapChain)) {
      GetInstance().SetOriginalWndProc((WNDPROC)SetWindowLongPtr(
        renderer.GetWindow(), GWLP_WNDPROC, (LONG_PTR)WndProcHook));
      
      menu.Initialize();
    } else {
      return GetInstance().GetOriginalPresent()(pSwapChain, SyncInterval, Flags);
    }
  }

  renderer.OnResize(pSwapChain);
  renderer.BeginFrame();
  menu.Render();
  renderer.EndFrame();

  return GetInstance().GetOriginalPresent()(pSwapChain, SyncInterval, Flags);
}

void HookManager::Install() {
  if (kiero::bind(8, (void**)&m_originalPresent, PresentHook) != kiero::Status::Success) {
    MessageBoxA(NULL, "failed to hook Present", "Error", MB_OK);
  }
}

void HookManager::Uninstall() {
  if (m_originalWndProc) {
    SetWindowLongPtr(Renderer::GetInstance().GetWindow(), GWLP_WNDPROC, (LONG_PTR)m_originalWndProc);
  }

  kiero::unbind(8);
}