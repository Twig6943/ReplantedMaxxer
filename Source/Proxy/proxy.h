#pragma once
#include "../includes.h"

extern "C" __declspec(dllexport) HRESULT __stdcall proxyDirectInput8Create(
    HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut,
    LPUNKNOWN punkOuter);