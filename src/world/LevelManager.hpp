// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LEVELMANAGER_HPP
#define LEVELMANAGER_HPP

#include "src/core/RenderManager.hpp"
#include "src/core/ResourceManager.hpp"
#include "src/world/Corridor.hpp"
#include "src/world/DungeonGenerator.hpp"
#include "src/world/Room.hpp"
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

class LevelManager {
private:
  std::vector<std::unique_ptr<Room>> rooms;
  std::vector<std::unique_ptr<Corridor>> corridors;

public:
  LevelManager() = default;

  void buildFromData(const DungeonData &data, ResourceManager &rm);

  template <typename Object> bool checkCollision(const Object &object) const {
    for (const auto &room : rooms)
      if (room->checkCollision(object))
        return true;
    for (const auto &corr : corridors)
      if (corr->checkCollision(object))
        return true;
    return false;
  }

  void submitRender(RenderManager &rm);

  std::optional<std::reference_wrapper<Room>>
  findRoomAt(sf::Vector2<float> position);
};

#endif // LEVELMANAGER_HPP
