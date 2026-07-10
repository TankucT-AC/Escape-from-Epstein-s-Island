// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <functional>
#include <vector>

class IDrawAble;

class RenderManager {
private:
  sf::RenderWindow &renderWindow;
  std::vector<std::reference_wrapper<IDrawAble>> renderQueue;

public:
  explicit RenderManager(sf::RenderWindow &initWindow);
  ~RenderManager() = default;

  void setCamera(const sf::View &camera);
  void clear();
  void submit(std::reference_wrapper<IDrawAble> object);
  void draw();
  void display();
};

#endif // RENDER_MANAGER_HPP
