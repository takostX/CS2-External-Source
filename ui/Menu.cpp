#include "Menu.h"
#include "../features/Features.h"
#include "Overlay.h"
#include "Theme.h"

Menu g_menu;

void Menu::handle_input() {
  if (GetAsyncKeyState(VK_INSERT) & 1) {
    is_open = !is_open;

    // Update overlay click-through
    LONG_PTR style = GetWindowLongPtr(g_overlay.hwnd, GWL_EXSTYLE);
    if (is_open) {
      style &= ~WS_EX_TRANSPARENT;
    } else {
      style |= WS_EX_TRANSPARENT;
    }
    SetWindowLongPtr(g_overlay.hwnd, GWL_EXSTYLE, style);
  }
}

void Menu::render() {
  if (!is_open)
    return;

  Theme::Setup();

  ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_Once);
  if (ImGui::Begin("nullsyntax - cs2", &is_open,
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoTitleBar)) {

    // Sidebar
    ImGui::BeginChild("sidebar", ImVec2(100, 0), true);
    const char *tabs[] = {"Aim", "Visuals", "Misc", "Settings"};
    for (int i = 0; i < 4; i++) {
      if (ImGui::Selectable(tabs[i], current_tab == i)) {
        current_tab = i;
      }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Content Area
    ImGui::BeginChild("content", ImVec2(0, 0), true);

    if (current_tab == 0) { // Aim
      Theme::Checkbox("Enable Aimbot", &Features::g_config.aim_enabled);
      Theme::SliderFloat("FOV", &Features::g_config.aim_fov, 1.0f, 180.0f,
                         "%.1f");
      Theme::SliderFloat("Smoothing", &Features::g_config.aim_smoothing, 1.0f,
                         20.0f, "%.1f");
    } else if (current_tab == 1) { // Visuals
      Theme::Checkbox("Enable ESP", &Features::g_config.esp_enabled);
      Theme::Checkbox("Box", &Features::g_config.esp_box);
      Theme::Checkbox("Health Bar", &Features::g_config.esp_health);
      Theme::Checkbox("Name", &Features::g_config.esp_name);
      Theme::Checkbox("Snaplines", &Features::g_config.esp_snaplines);
    } else if (current_tab == 2) { // Misc
      Theme::Checkbox("Triggerbot", &Features::g_config.trigger_enabled);
      Theme::Checkbox("Bunnyhop", &Features::g_config.bh_enabled);
      Theme::SliderFloat("Miss Chance", &Features::g_config.miss_chance, 0.0f,
                         100.0f, "%.0f%%");
    } else if (current_tab == 3) { // Settings
      if (ImGui::Button("Unload", ImVec2(80, 25))) {
        g_overlay.is_running = false;
      }
    }

    ImGui::EndChild();
    ImGui::End();
  }
}
