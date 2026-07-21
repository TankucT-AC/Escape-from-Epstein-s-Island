// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "LevelManager.hpp"
#include <functional>

void LevelManager::buildFromData(const DungeonData &data, ResourceManager &rm) {
  m_rooms.clear();
  m_corridors.clear();

  const int doorHalfW = data.corridorWidth / 2;

  for (const auto &rp : data.rooms) {
    m_rooms.push_back(std::make_unique<Room>(
        rp.prefabIndex, sf::Vector2<int>{rp.tileX, rp.tileY}, rp.connectUp,
        rp.connectDown, rp.connectLeft, rp.connectRight, doorHalfW, rm));
  }

  for (const auto &cp : data.corridors) {
    m_corridors.push_back(std::make_unique<Corridor>(
        cp.isHorizontal, cp.tileX, cp.tileY, cp.width, cp.height, rm));
  }
}

void LevelManager::submitRender(RenderManager &rm) {
  for (const auto &room : m_rooms)
    room->submitRender(rm);
  for (const auto &corr : m_corridors)
    corr->submitRender(rm);
}

std::optional<std::reference_wrapper<Room>>
LevelManager::findRoomAt(sf::Vector2<float> position) {
  for (const auto &room : m_rooms)
    if (room->isObjectInRoom(position))
      return *room;
  return std::nullopt;
}

std::optional<std::reference_wrapper<Chest>>
LevelManager::findChestAt(sf::Vector2<float> worldPos) {
  for (const auto &room : m_rooms) {
    std::optional<std::reference_wrapper<Chest>> chestOpt = room->getChest();
    if (!chestOpt.has_value())
      continue;

    Chest &chest = chestOpt.value().get();
    if (!chest.isOpen() && chest.getHitbox().contains(worldPos))
      return chest;
  }
  return std::nullopt;
}
