#include "features/Features.h"
#include "sdk/Memory.h"
#include "ui/Menu.h"
#include "ui/Overlay.h"

Memory *g_mem = nullptr;

int main() {
  std::cout << "[+] Looking for CS2..." << std::endl;

  g_mem = new Memory("cs2.exe");
  while (!g_mem->process_id) {
    delete g_mem;
    g_mem = new Memory("cs2.exe");
    Sleep(1000);
  }

  std::cout << "[+] Found CS2 (PID: " << g_mem->process_id << ")" << std::endl;

  if (!g_overlay.init_window()) {
    std::cout << "[-] Failed to init window." << std::endl;
    return 1;
  }

  if (!g_overlay.init_dx11()) {
    std::cout << "[-] Failed to init DX11." << std::endl;
    return 1;
  }

  std::cout << "[+] Overlay initialized. Press INSERT for menu." << std::endl;

  while (g_overlay.is_running) {
    g_menu.handle_input();
    g_overlay.render();
  }

  g_overlay.clean_up();
  delete g_mem;
  return 0;
}
