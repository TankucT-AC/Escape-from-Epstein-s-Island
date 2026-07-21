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
#include <memory>
#include <optional>
#include <vector>

class LevelManager {
private:
  std::vector<std::unique_ptr<Room>> m_rooms;
  std::vector<std::unique_ptr<Corridor>> m_corridors;

public:
  LevelManager() = default;

  void buildFromData(const DungeonData &data, ResourceManager &rm);

  template <typename Object> bool checkCollision(const Object &object) const {
    auto hb = object.getHitbox();
    for (const auto &room : m_rooms)
      if (room->checkCollision(object))
        return true;
    for (const auto &corr : m_corridors)
      if (corr->checkCollision(object))
        return true;
    return false;
  }

  void submitRender(RenderManager &rm);

  std::optional<std::reference_wrapper<Room>>
  findRoomAt(sf::Vector2<float> position);

  std::optional<std::reference_wrapper<Chest>>
  findChestAt(sf::Vector2<float> worldPos);

  const std::vector<std::unique_ptr<Room>> &getRooms() const { return m_rooms; }
};

#endif // LEVELMANAGER_HPP
