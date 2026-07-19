// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CORRIDOR_HPP
#define CORRIDOR_HPP

#include "src/core/RenderManager.hpp"
#include "src/core/ResourceManager.hpp"
#include "src/world/Floor.hpp"
#include "src/world/Wall.hpp"
#include <memory>

class Corridor {
private:
  std::vector<std::unique_ptr<Wall>> walls;
  std::vector<std::unique_ptr<Floor>> floors;

public:
  // Строит коридор заданных размеров (в тайлах).
  // isHorizontal: true  → верхняя/нижняя стенки, проход по центру
  //               false → левая/правая  стенки, проход по центру
  // width, height включают стенки.
  Corridor(bool isHorizontal, int tileX, int tileY, int width, int height,
           ResourceManager &rm);

  template <typename Object> bool checkCollision(const Object &object) const {
    auto objectHitbox = object.getHitbox();
    for (const auto &wall : walls) {
      auto wallHitbox = wall->getHitbox();
      if (wallHitbox.intersects(objectHitbox))
        return true;
    }
    return false;
  }

  void submitRender(RenderManager &rm);
};

#endif // CORRIDOR_HPP