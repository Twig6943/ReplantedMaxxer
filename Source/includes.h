#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "kiero.h"

#include "Renderer/renderer.h"
#include "UI/menu.h"

#include "Hooks/hook_manager.h"
#include "Proxy/proxy.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

typedef HRESULT(__stdcall* PresentFunc)(IDXGISwapChain* pSwapChain,
                                    UINT SyncInterval, UINT Flags);
typedef uintptr_t PTR;