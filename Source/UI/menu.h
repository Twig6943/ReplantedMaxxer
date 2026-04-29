#pragma once
#include "../includes.h"

class Menu {
public:
  static Menu& GetInstance() {
    static Menu instance;
    return instance;
  }

  void Initialize();
  void Render();
  bool IsVisible() const { return m_showMenu; }
  void Toggle() { m_showMenu = !m_showMenu; }

private:
  Menu() = default;
  ~Menu() = default;
  Menu(const Menu&) = delete;
  Menu& operator=(const Menu&) = delete;

  void SetupStyle();

  bool m_showMenu = true;
};