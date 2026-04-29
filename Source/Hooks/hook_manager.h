#pragma once
#include "../includes.h"

class HookManager {
public:
  static HookManager& GetInstance() {
    static HookManager instance;
    return instance;
  }

  void Install();
  void Uninstall();

  using PresentFunction = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
  
  PresentFunction GetOriginalPresent() const { return m_originalPresent; }
  void SetOriginalPresent(PresentFunction present) { m_originalPresent = present; }
  
  WNDPROC GetOriginalWndProc() const { return m_originalWndProc; }
  void SetOriginalWndProc(WNDPROC wndproc) { m_originalWndProc = wndproc; }

private:
  HookManager() = default;
  ~HookManager() = default;
  HookManager(const HookManager&) = delete;
  HookManager& operator=(const HookManager&) = delete;

  static LRESULT CALLBACK WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

  WNDPROC m_originalWndProc = nullptr;
  PresentFunction m_originalPresent = nullptr;
};