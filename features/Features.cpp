#include "Features.h"
#include "../imgui/imgui.h"
#include "../sdk/Memory.h"
#include "../sdk/Offsets.h"

namespace Features {
Config g_config;

void run() {
  if (!g_mem || !g_mem->process_handle)
    return;

  uintptr_t client_base = g_mem->get_module_base("client.dll");
  if (!client_base)
    return;

  Matrix4x4 view_matrix =
      g_mem->read<Matrix4x4>(client_base + Offsets::dwViewMatrix);
  uintptr_t entity_list =
      g_mem->read<uintptr_t>(client_base + Offsets::dwEntityList);
  uintptr_t local_player_controller =
      g_mem->read<uintptr_t>(client_base + Offsets::dwLocalPlayerController);
  uintptr_t local_pawn_handle =
      g_mem->read<uintptr_t>(local_player_controller + Offsets::m_hPawn);

  // Get Local Pawn address
  uintptr_t list_entry_local = g_mem->read<uintptr_t>(
      entity_list + (8 * (local_pawn_handle & 0x7FFF) >> 9) + 16);
  uintptr_t local_pawn = g_mem->read<uintptr_t>(
      list_entry_local + 120 * (local_pawn_handle & 0x1FF));
  if (!local_pawn)
    return;

  int local_team = g_mem->read<int>(local_pawn + Offsets::m_iTeamNum);
  int width = GetSystemMetrics(SM_CXSCREEN);
  int height = GetSystemMetrics(SM_CYSCREEN);
  Vector2 screen_center = {width / 2.0f, height / 2.0f};

  float best_fov = g_config.aim_fov;
  Vector2 best_target = {0, 0};

  ImDrawList *draw_list = ImGui::GetBackgroundDrawList();

  for (int i = 1; i < 64; i++) {
    uintptr_t list_entry =
        g_mem->read<uintptr_t>(entity_list + (8 * (i & 0x7FFF) >> 9) + 16);
    if (!list_entry)
      continue;

    uintptr_t controller =
        g_mem->read<uintptr_t>(list_entry + 120 * (i & 0x1FF));
    if (!controller || controller == local_player_controller)
      continue;

    uintptr_t pawn_handle =
        g_mem->read<uintptr_t>(controller + Offsets::m_hPawn);
    uintptr_t list_entry_pawn = g_mem->read<uintptr_t>(
        entity_list + (8 * (pawn_handle & 0x7FFF) >> 9) + 16);
    if (!list_entry_pawn)
      continue;

    uintptr_t pawn =
        g_mem->read<uintptr_t>(list_entry_pawn + 120 * (pawn_handle & 0x1FF));
    if (!pawn || pawn == local_pawn)
      continue;

    int health = g_mem->read<int>(pawn + Offsets::m_iHealth);
    if (health <= 0 || health > 100)
      continue;

    int team = g_mem->read<int>(pawn + Offsets::m_iTeamNum);
    if (team == local_team)
      continue;

    uintptr_t scene_node =
        g_mem->read<uintptr_t>(pawn + Offsets::m_pGameSceneNode);
    Vector3 origin = g_mem->read<Vector3>(scene_node + Offsets::m_vecAbsOrigin);
    Vector3 head_pos =
        origin +
        Vector3(0, 0,
                75.0f); // Bone 6 is better, but using height for stability

    Vector2 screen_origin, screen_head;
    if (Math::world_to_screen(origin, screen_origin, view_matrix, width,
                              height) &&
        Math::world_to_screen(head_pos, screen_head, view_matrix, width,
                              height)) {

      float box_h = screen_origin.y - screen_head.y;
      float box_w = box_h / 2.0f;
      float box_x = screen_head.x - box_w / 2.0f;
      float box_y = screen_head.y;

      // ESP logic
      if (g_config.esp_enabled) {
        ImColor purple = ImColor(0.7f, 0.4f, 1.0f, 1.0f);
        if (g_config.esp_box) {
          draw_list->AddRect(ImVec2(box_x, box_y),
                             ImVec2(box_x + box_w, box_y + box_h), purple, 0, 0,
                             1.0f);
        }
        if (g_config.esp_health) {
          float health_pct = health / 100.0f;
          ImColor health_col =
              ImColor(1.0f - health_pct, health_pct, 0.0f, 1.0f);
          draw_list->AddRectFilled(ImVec2(box_x - 6, box_y),
                                   ImVec2(box_x - 2, box_y + box_h),
                                   ImColor(0, 0, 0, 150));
          draw_list->AddRectFilled(
              ImVec2(box_x - 5, box_y + box_h - (box_h * health_pct)),
              ImVec2(box_x - 3, box_y + box_h), health_col);
        }
        if (g_config.esp_name) {
          char name[128] = {0};
          g_mem->read_raw(controller + Offsets::m_iszPlayerName, name,
                          sizeof(name));
          ImVec2 text_size = ImGui::CalcTextSize(name);
          // Draw shadow
          draw_list->AddText(
              ImVec2(box_x + box_w / 2.0f - text_size.x / 2.0f + 1, box_y - 14),
              ImColor(0, 0, 0, 200), name);
          draw_list->AddText(
              ImVec2(box_x + box_w / 2.0f - text_size.x / 2.0f, box_y - 15),
              ImColor(255, 255, 255), name);
        }
        if (g_config.esp_snaplines) {
          draw_list->AddLine(ImVec2(width / 2.0f, height),
                             ImVec2(screen_origin.x, screen_origin.y),
                             ImColor(255, 255, 255, 150));
        }
      }

      // Aimbot selection
      if (g_config.aim_enabled) {
        float dist = std::sqrt(std::pow(screen_head.x - screen_center.x, 2) +
                               std::pow(screen_head.y - screen_center.y, 2));
        if (dist < best_fov) {
          best_fov = dist;
          best_target = screen_head;
        }
      }
    }
  }

  // Aimbot execution
  if (g_config.aim_enabled && (GetAsyncKeyState(g_config.aim_key) & 0x8000)) {
    if (best_target.x != 0) {
      float target_x =
          (best_target.x - screen_center.x) / g_config.aim_smoothing;
      float target_y =
          (best_target.y - screen_center.y) / g_config.aim_smoothing;

      INPUT input = {0};
      input.type = INPUT_MOUSE;
      input.mi.dwFlags = MOUSEEVENTF_MOVE;
      input.mi.dx = (LONG)target_x;
      input.mi.dy = (LONG)target_y;
      SendInput(1, &input, sizeof(input));
    }
  }

  // Bunnyhop execution
  if (g_config.bh_enabled && (GetAsyncKeyState(VK_SPACE) & 0x8000)) {
    uint32_t flags = g_mem->read<uint32_t>(local_pawn + Offsets::m_fFlags);
    if (flags & (1 << 0)) { // FL_ONGROUND
      keybd_event(VK_SPACE, 0x39, 0, 0);
      Sleep(1);
      keybd_event(VK_SPACE, 0x39, KEYEVENTF_KEYUP, 0);
    }
  }

  // Triggerbot execution
  if (g_config.trigger_enabled &&
      (GetAsyncKeyState(g_config.trigger_key) & 0x8000)) {
    int crosshair_id = g_mem->read<int>(local_pawn + Offsets::m_iIDEntIndex);
    if (crosshair_id > 0 && crosshair_id <= 64) {
      uintptr_t list_entry = g_mem->read<uintptr_t>(
          entity_list + (8 * (crosshair_id & 0x7FFF) >> 9) + 16);
      uintptr_t controller =
          g_mem->read<uintptr_t>(list_entry + 120 * (crosshair_id & 0x1FF));
      if (controller) {
        int team = g_mem->read<int>(controller + Offsets::m_iTeamNum);
        if (team != local_team) {
          // Miss chance check
          if (g_config.miss_chance > 0.0f) {
            if ((rand() % 100) < (int)g_config.miss_chance)
              return; // We missed!
          }

          mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
          Sleep(1);
          mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
          Sleep(25);
        }
      }
    }
  }
}
} // namespace Features
