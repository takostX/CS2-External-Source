#pragma once
#include "../imgui/imgui.h"
#include <windows.h>


class Menu {
public:
  bool is_open = false;
  int current_tab = 0;

  void render();
  void handle_input();
};

extern Menu g_menu;
