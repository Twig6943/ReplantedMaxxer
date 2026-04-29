#include "menu.h"
#include "../Renderer/renderer.h"

void Menu::Initialize() {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();

  SetupStyle();

  ImGui_ImplWin32_Init(Renderer::GetInstance().GetWindow());
  ImGui_ImplDX11_Init(Renderer::GetInstance().GetDevice(), Renderer::GetInstance().GetContext());
}

void Menu::SetupStyle() {
  ImGui::StyleColorsDark();
  
  float dpiScale = 1.0f;
  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(dpiScale);
  style.FontScaleDpi = dpiScale;

  style.FrameBorderSize = 1.0f;
  style.FrameRounding = 2.0f;
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

  style.Colors[ImGuiCol_Button] = ImColor(50, 200, 90, 255).Value;
  style.Colors[ImGuiCol_ButtonHovered] = ImColor(70, 240, 120, 230).Value;
  style.Colors[ImGuiCol_ButtonActive] = ImColor(40, 170, 80, 255).Value;
  style.Colors[ImGuiCol_CheckMark] = ImColor(150, 255, 160, 255).Value;
  style.Colors[ImGuiCol_FrameBg] = ImColor(25, 35, 25, 160).Value;
  style.Colors[ImGuiCol_FrameBgActive] = ImColor(60, 100, 60, 200).Value;
  style.Colors[ImGuiCol_FrameBgHovered] = ImColor(45, 75, 45, 140).Value;
  style.Colors[ImGuiCol_ResizeGrip] = ImColor(90, 230, 130, 150).Value;
  style.Colors[ImGuiCol_SliderGrab] = ImColor(80, 220, 120, 255).Value;
  style.Colors[ImGuiCol_SliderGrabActive] = ImColor(110, 255, 150, 255).Value;
  style.Colors[ImGuiCol_Tab] = ImColor(60, 180, 100, 230).Value;
  style.Colors[ImGuiCol_TabHovered] = ImColor(80, 240, 130, 230).Value;
  style.Colors[ImGuiCol_TabSelected] = ImColor(70, 200, 110, 255).Value;
  style.Colors[ImGuiCol_TitleBgActive] = ImColor(60, 180, 100, 255).Value;
  style.Colors[ImGuiCol_WindowBg] = ImColor(15, 25, 15, 255).Value;
}

void Menu::Render() {
  if (!m_showMenu) return;

  ImGui::Begin("ImGui Window");
  ImGui::Text("(Press F1 to toggle)");

  ImGui::Button("Eject Dll");

  ImGui::End();
}