// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Corridor.hpp"
#include "src/core/config.hpp"

Corridor::Corridor(bool isHorizontal, int tileX, int tileY, int width,
                   int height, ResourceManager &rm) {
  const sf::Texture &wallTex = rm.getTexture(config::DEFAULT_WALL_TEXTURE);
  const sf::Texture &floorTex = rm.getTexture(config::DESERT_FLOOR_TEXTURE);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int gx = tileX + x;
      int gy = tileY + y;
      sf::Vector2<float> pos = {
          gx * config::TILE_SIZE + config::TILE_SIZE / 2.f,
          gy * config::TILE_SIZE + config::TILE_SIZE / 2.f};

      bool isWall = false;
      if (isHorizontal) {
        // Стенки сверху и снизу
        isWall = (y == 0 || y == height - 1);
      } else {
        // Стенки слева и справа
        isWall = (x == 0 || x == width - 1);
      }

      if (isWall) {
        walls.push_back(std::make_unique<Wall>(wallTex, pos, false));
      } else {
        floors.push_back(std::make_unique<Floor>(floorTex, pos));
      }
    }
  }
}

void Corridor::submitRender(RenderManager &rm) {
  for (const auto &w : walls)
    rm.submit(*w);
  for (const auto &f : floors)
    rm.submit(*f);
}