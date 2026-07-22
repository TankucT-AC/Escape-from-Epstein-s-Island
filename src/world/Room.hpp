// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ROOM_HPP
#define ROOM_HPP

#include "src/core/RenderManager.hpp"
#include "src/core/ResourceManager.hpp"
#include "src/game/Player.hpp"
#include "src/world/Chest.hpp"
#include "src/world/Floor.hpp"
#include "src/world/Gate.hpp"
#include "src/world/Wall.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>

enum class RoomElements { FLOOR, WALL, TEMP_FLAG, EMPTY };

// Только три типа: Spawn, Combat, Treasure.
// Treasure = единственная комната с гарантированным сундуком (префаб TREASURE).
enum class RoomType { Spawn, Combat, Treasure, Portal };

enum class CombatState { Inactive, Active, Cleared };

struct GateSlot {
  sf::Vector2<float> worldPos;
  bool isHorizontal;
  int doorWidth;
};

class Room {
private:
  std::vector<std::unique_ptr<Wall>> m_walls;
  std::vector<std::unique_ptr<Floor>> m_floors;
  std::vector<std::unique_ptr<Gate>> m_gates;
  std::unique_ptr<Chest> m_chest;

  sf::Vector2<float> m_chestPos;
  bool m_hasChestSlot = false;

  sf::Vector2<int> m_tilePos;
  RoomType m_roomType;
  CombatState m_combatState;
  std::vector<sf::Vector2<float>> m_spawnPoints;

  void buildFromPrefab(uint8_t prefabIndex, bool cUp, bool cDown, bool cLeft,
                       bool cRight, int doorHalfW, ResourceManager &rm);

public:
  Room(const std::vector<std::vector<int>> &InitBlueprint,
       sf::Vector2<float> InitPos, ResourceManager &rm);

  Room(uint8_t prefabIndex, sf::Vector2<int> tilePos, bool cUp, bool cDown,
       bool cLeft, bool cRight, int doorHalfW, ResourceManager &rm);

  template <typename Object> bool checkCollision(const Object &object) const {
    auto hb = object.getHitbox();
    for (const auto &w : m_walls)
      if (w->getHitbox().intersects(hb))
        return true;
    for (const auto &g : m_gates)
      if (!g->isOpen() && g->getHitbox().intersects(hb))
        return true;
    return false;
  }

  bool doesPlayerOverlapGates(const Player &player) const {
    auto phb = player.getHitbox();
    for (const auto &g : m_gates)
      if (g->getHitbox().intersects(phb))
        return true;
    return false;
  }

  void closeAllGates() {
    for (auto &g : m_gates)
      g->setClosed();
  }
  void openAllGates() {
    for (auto &g : m_gates)
      g->setOpen();
  }

  void spawnChest(ResourceManager &rm);

  void submitRender(RenderManager &RenderManager);

  bool isObjectInRoom(sf::Vector2<float> position);

  RoomType getRoomType() const { return m_roomType; }
  CombatState getCombatState() const { return m_combatState; }
  void setCombatState(CombatState s) { m_combatState = s; }
  const std::vector<sf::Vector2<float>> &getSpawnPoints() const {
    return m_spawnPoints;
  }
  bool hasChestSlot() const { return m_hasChestSlot; }
  sf::Vector2<float> getChestPos() const { return m_chestPos; }
  std::optional<std::reference_wrapper<Chest>> getChest() {
    if (!m_chest)
      return std::nullopt;
    return *m_chest;
  }
  sf::Vector2<float> getPosition() const {
    return m_walls.front()->getPosition();
  }

  // Только TREASURE (3) даёт тип Treasure.
  // SPAWN (1) → Spawn, COMBAT (2) и ARENA (6) → Combat, остальное → Combat.
  static RoomType typeFromPrefab(uint8_t prefabIndex);
};

#endif // ROOM_HPP