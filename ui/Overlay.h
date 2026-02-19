#pragma once
#include "../imgui/backends/imgui_impl_dx11.h"
#include "../imgui/backends/imgui_impl_win32.h"
#include "../imgui/imgui.h"
#include <d3d11.h>
#include <windows.h>


class Overlay {
public:
  HWND hwnd;
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;
  IDXGISwapChain *swap_chain;
  ID3D11RenderTargetView *render_target_view;

  bool init_window();
  bool init_dx11();
  void render();
  void clean_up();

  bool is_running = true;
};

extern Overlay g_overlay;
