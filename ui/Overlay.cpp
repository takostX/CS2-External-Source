#include "Overlay.h"
#include <dwmapi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")

Overlay g_overlay;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    return true;

  switch (msg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Overlay::init_window() {
  WNDCLASSEX wc = {sizeof(WNDCLASSEX),
                   CS_HREDRAW | CS_VREDRAW,
                   WindowProc,
                   0,
                   0,
                   GetModuleHandle(NULL),
                   NULL,
                   NULL,
                   NULL,
                   NULL,
                   "Overlay",
                   NULL};
  RegisterClassEx(&wc);

  hwnd = CreateWindowEx(
      WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, "Overlay",
      "CS2 External Analysis", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
      GetSystemMetrics(SM_CYSCREEN), NULL, NULL, wc.hInstance, NULL);

  SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_COLORKEY);

  MARGINS margins = {-1};
  DwmExtendFrameIntoClientArea(hwnd, &margins);

  ShowWindow(hwnd, SW_SHOW);
  return true;
}

bool Overlay::init_dx11() {
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hwnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  if (D3D11CreateDeviceAndSwapChain(
          NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION,
          &sd, &swap_chain, &device, NULL, &device_context) != S_OK)
    return false;

  ID3D11Texture2D *back_buffer;
  swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
  device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
  back_buffer->Release();

  ImGui::CreateContext();
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(device, device_context);

  return true;
}

void Overlay::render() {
  MSG msg;
  while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    if (msg.message == WM_QUIT)
      is_running = false;
  }

  if (!is_running)
    return;

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // Interaction handling (Menu and Features)
  extern void RunFeaturesAndMenu();
  RunFeaturesAndMenu();

  ImGui::Render();
  const float clear_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  device_context->OMSetRenderTargets(1, &render_target_view, NULL);
  device_context->ClearRenderTargetView(render_target_view, clear_color);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  swap_chain->Present(1, 0);
}

void Overlay::clean_up() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  if (render_target_view)
    render_target_view->Release();
  if (swap_chain)
    swap_chain->Release();
  if (device_context)
    device_context->Release();
  if (device)
    device->Release();
  DestroyWindow(hwnd);
  UnregisterClass("Overlay", GetModuleHandle(NULL));
}
