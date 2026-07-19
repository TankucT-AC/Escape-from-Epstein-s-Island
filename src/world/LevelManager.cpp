// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "LevelManager.hpp"
#include <SFML/System/Vector2.hpp>

void LevelManager::buildFromData(const DungeonData &data, ResourceManager &rm) {
  rooms.clear();
  corridors.clear();

  const int doorHalfW = data.corridorWidth / 2;

  for (const auto &rp : data.rooms) {
    rooms.push_back(std::make_unique<Room>(
        rp.prefabIndex, sf::Vector2<int>{rp.tileX, rp.tileY}, rp.connectUp,
        rp.connectDown, rp.connectLeft, rp.connectRight, doorHalfW, rm));
  }

  for (const auto &cp : data.corridors) {
    corridors.push_back(std::make_unique<Corridor>(
        cp.isHorizontal, cp.tileX, cp.tileY, cp.width, cp.height, rm));
  }
}

void LevelManager::submitRender(RenderManager &rm) {
  for (const auto &room : rooms)
    room->submitRender(rm);
  for (const auto &corr : corridors)
    corr->submitRender(rm);
}

std::optional<std::reference_wrapper<Room>>
LevelManager::findRoomAt(sf::Vector2<float> position) {
  for (const auto &room : rooms) {
    if (room->isObjectInRoom(position)) {
      return *room;
    }
  }

  return std::nullopt;
}
