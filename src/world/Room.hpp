// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ROOM_HPP
#define ROOM_HPP

#include "src/core/RenderManager.hpp"
#include "src/core/ResourceManager.hpp"
#include "src/world/Floor.hpp"
#include "src/world/Wall.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <memory>

enum class RoomElements { FLOOR, WALL, TEMP_FLAG, EMPTY };

class Room {
private:
  std::vector<std::unique_ptr<Wall>> walls;
  std::vector<std::unique_ptr<Floor>> floors;
  sf::Vector2<float> position;

public:
  // TODO: удалить старый конструктор
  Room(const std::vector<std::vector<int>> &InitBlueprint,
       sf::Vector2<float> InitPos, ResourceManager &rm);

  Room(uint8_t prefabIndex, sf::Vector2<int> tilePos, bool cUp, bool cDown,
       bool cLeft, bool cRight, int doorHalfW, ResourceManager &rm);

  template <typename Object> bool checkCollision(const Object &object) const {
    auto objectHitbox = object.getHitbox();
    for (const auto &wall : walls) {
      auto wallHitbox = wall->getHitbox();
      if (wallHitbox.intersects(objectHitbox))
        return true;
    }
    return false;
  }

  void submitRender(RenderManager &RenderManager);
  sf::Vector2<float> getPosition() const {
    return walls.front()->getPosition();
  }

  bool isObjectInRoom(sf::Vector2<float> position);
};

#endif // ROOM_HPP
