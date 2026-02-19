#pragma once
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

namespace Theme {
inline void Setup() {
  auto &style = ImGui::GetStyle();
  auto colors = style.Colors;

  style.WindowRounding = 4.0f;
  style.FrameRounding = 2.0f;
  style.PopupRounding = 4.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabRounding = 4.0f;
  style.WindowBorderSize = 0.0f;
  style.FrameBorderSize = 0.0f;

  ImVec4 background = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
  ImVec4 accent = ImVec4(0.7f, 0.4f, 1.0f, 1.0f);
  ImVec4 text = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

  colors[ImGuiCol_WindowBg] = background;
  colors[ImGuiCol_Header] = accent;
  colors[ImGuiCol_HeaderActive] = accent;
  colors[ImGuiCol_HeaderHovered] = accent;
  colors[ImGuiCol_CheckMark] = accent;
  colors[ImGuiCol_SliderGrab] = accent;
  colors[ImGuiCol_SliderGrabActive] = accent;
  colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
  colors[ImGuiCol_ButtonActive] = accent;
  colors[ImGuiCol_ButtonHovered] = accent;
  colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
  colors[ImGuiCol_Text] = text;
  colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
}

// Custom 14x14 Square Checkbox
inline bool Checkbox(const char *label, bool *v) {
  ImGuiWindow *window = ImGui::GetCurrentWindow();
  if (window->SkipItems)
    return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);
  const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

  const float square_sz = 14.0f;
  const ImRect check_bb(window->DC.CursorPos,
                        window->DC.CursorPos + ImVec2(square_sz, square_sz));
  ImGui::ItemSize(check_bb, style.FramePadding.y);
  if (!ImGui::ItemAdd(check_bb, id))
    return false;

  bool hovered, held;
  bool pressed = ImGui::ButtonBehavior(check_bb, id, &hovered, &held);
  if (pressed) {
    *v = !(*v);
    ImGui::MarkItemEdited(id);
  }

  ImGui::RenderFrame(check_bb.Min, check_bb.Max,
                     ImGui::GetColorU32(ImGuiCol_FrameBg), true, 1.0f);
  if (*v) {
    window->DrawList->AddRectFilled(check_bb.Min + ImVec2(2, 2),
                                    check_bb.Max - ImVec2(2, 2),
                                    ImGui::GetColorU32(ImGuiCol_CheckMark));
  }

  if (label_size.x > 0.0f) {
    ImGui::RenderText(
        ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y),
        label);
  }

  return pressed;
}

// Custom Thin Slider with Circular Grab
inline bool SliderFloat(const char *label, float *v, float v_min, float v_max,
                        const char *format = "%.3f") {
  ImGuiWindow *window = ImGui::GetCurrentWindow();
  if (window->SkipItems)
    return false;

  ImGuiContext &g = *GImGui;
  const ImGuiStyle &style = g.Style;
  const ImGuiID id = window->GetID(label);
  const float w = ImGui::CalcItemWidth();

  const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
  const ImRect frame_bb(
      window->DC.CursorPos,
      window->DC.CursorPos +
          ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
  const ImRect total_bb(frame_bb.Min,
                        frame_bb.Max +
                            ImVec2(label_size.x > 0.0f
                                       ? style.ItemInnerSpacing.x + label_size.x
                                       : 0.0f,
                                   0.0f));

  ImGui::ItemSize(total_bb, style.FramePadding.y);
  if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
    return false;

  const bool hovered =
      ImGui::ItemHoverable(frame_bb, id, g.NextItemData.ItemFlags);
  bool temp_v_changed =
      ImGui::SliderBehavior(frame_bb, id, ImGuiDataType_Float, v, &v_min,
                            &v_max, format, ImGuiSliderFlags_None, &frame_bb);
  if (temp_v_changed)
    ImGui::MarkItemEdited(id);

  // Draw track (thin line)
  float track_height = 2.0f;
  float track_y = frame_bb.Min.y + (frame_bb.Max.y - frame_bb.Min.y) / 2.0f -
                  track_height / 2.0f;
  window->DrawList->AddRectFilled(
      ImVec2(frame_bb.Min.x, track_y),
      ImVec2(frame_bb.Max.x, track_y + track_height),
      ImGui::GetColorU32(ImGuiCol_FrameBg), 1.0f);

  // Draw grab (circular)
  float grab_pos = (*v - v_min) / (v_max - v_min);
  float grab_x = frame_bb.Min.x + grab_pos * (frame_bb.Max.x - frame_bb.Min.x);
  window->DrawList->AddCircleFilled(
      ImVec2(grab_x, track_y + track_height / 2.0f), 5.0f,
      ImGui::GetColorU32(ImGuiCol_SliderGrab));

  // Draw value on the right
  char value_buf[64];
  sprintf_s(value_buf, format, *v);
  ImGui::RenderText(
      ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y),
      value_buf);

  return temp_v_changed;
}
} // namespace Theme
