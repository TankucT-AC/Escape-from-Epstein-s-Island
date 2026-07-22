// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef COMBATMANAGER_HPP
#define COMBATMANAGER_HPP

#include "ResourceManager.hpp"
#include "src/game/Enemy.hpp"
#include "src/game/Player.hpp"
#include "src/world/Room.hpp"
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

class CombatManager {
private:
  std::optional<std::reference_wrapper<Room>> m_activeRoom;
  int m_combatWave = 0;
  bool m_gatesClosed = false;
  static const int m_maxWaves = 3;

public:
  CombatManager() = default;

  void activate(Room &room);
  bool tryCloseGates(Room &room, const Player &player);
  void spawnWave(Room &room, std::vector<std::unique_ptr<Enemy>> &enemies,
                 ResourceManager &rm);
  void clearRoom(Room &room);
  bool checkWaveComplete(std::vector<std::unique_ptr<Enemy>> &enemies);
  bool hasActiveRoom() const { return m_activeRoom.has_value(); }
  bool isSpawnPending() const {
    return hasActiveRoom() && !m_gatesClosed;
  }
  Room &activeRoom() { return m_activeRoom.value().get(); }
  int currentWave() const { return m_combatWave; }
  int maxWaves() const { return m_maxWaves; }
};

#endif