#pragma once
#include "../math/Vector.h"
#include <windows.h>

namespace Features {
struct Config {
  bool esp_enabled = true;
  bool esp_box = true;
  bool esp_health = true;
  bool esp_name = true;
  bool esp_snaplines = false;

  bool aim_enabled = true;
  float aim_fov = 10.0f;
  float aim_smoothing = 5.0f;
  int aim_key = VK_LBUTTON;

  bool trigger_enabled = false;
  int trigger_key = VK_XBUTTON2;

  bool bh_enabled = false;
  float miss_chance = 0.0f;
};

extern Config g_config;

void run();
} // namespace Features
