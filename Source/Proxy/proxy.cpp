#include "proxy.h"

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