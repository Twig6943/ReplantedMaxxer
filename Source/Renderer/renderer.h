#pragma once
#include "../includes.h"

class Renderer {
public:
  static Renderer& GetInstance() {
    static Renderer instance;
    return instance;
  }

  bool Initialize(IDXGISwapChain* pSwapChain);
  void Cleanup();
  void OnResize(IDXGISwapChain* pSwapChain);
  
  ID3D11Device* GetDevice() const { return m_pDevice; }
  ID3D11DeviceContext* GetContext() const { return m_pContext; }
  ID3D11RenderTargetView* GetRenderTargetView() const { return m_mainRenderTargetView; }
  HWND GetWindow() const { return m_window; }
  bool IsInitialized() const { return m_initialized; }

  void BeginFrame();
  void EndFrame();

private:
  Renderer() = default;
  ~Renderer() { Cleanup(); }
  
  void CreateRenderTarget(IDXGISwapChain* pSwapChain);
  void CleanupRenderTarget();

  ID3D11Device* m_pDevice = nullptr;
  ID3D11DeviceContext* m_pContext = nullptr;
  ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;
  HWND m_window = nullptr;
  bool m_initialized = false;
  
  UINT m_lastWidth = 0;
  UINT m_lastHeight = 0;
};