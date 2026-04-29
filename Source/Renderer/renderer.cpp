#include "renderer.h"

bool Renderer::Initialize(IDXGISwapChain* pSwapChain) {
  if (m_initialized) return true;

  if (FAILED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&m_pDevice)))
    return false;

  m_pDevice->GetImmediateContext(&m_pContext);

  DXGI_SWAP_CHAIN_DESC sd;
  pSwapChain->GetDesc(&sd);
  m_window = sd.OutputWindow;

  CreateRenderTarget(pSwapChain);
  m_initialized = true;
  
  return true;
}

void Renderer::Cleanup() {
  CleanupRenderTarget();
  
  if (m_pContext) {
    m_pContext->Release();
    m_pContext = nullptr;
  }
  
  if (m_pDevice) {
    m_pDevice->Release();
    m_pDevice = nullptr;
  }
}

void Renderer::CreateRenderTarget(IDXGISwapChain* pSwapChain) {
  CleanupRenderTarget();
  
  ID3D11Texture2D* pBackBuffer = nullptr;
  if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), 
                                      (LPVOID*)&pBackBuffer))) {
    m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_mainRenderTargetView);
    pBackBuffer->Release();
  }
}

void Renderer::CleanupRenderTarget() {
  if (m_mainRenderTargetView) {
    m_mainRenderTargetView->Release();
    m_mainRenderTargetView = nullptr;
  }
}

void Renderer::OnResize(IDXGISwapChain* pSwapChain) {
  DXGI_SWAP_CHAIN_DESC sd;
  pSwapChain->GetDesc(&sd);
  
  if (sd.BufferDesc.Width != m_lastWidth || sd.BufferDesc.Height != m_lastHeight) {
    m_lastWidth = sd.BufferDesc.Width;
    m_lastHeight = sd.BufferDesc.Height;
    
    CleanupRenderTarget();
    CreateRenderTarget(pSwapChain);
  }
}

void Renderer::BeginFrame() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
}

void Renderer::EndFrame() {
  ImGui::Render();
  
  if (m_mainRenderTargetView)
    m_pContext->OMSetRenderTargets(1, &m_mainRenderTargetView, nullptr);
  
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}