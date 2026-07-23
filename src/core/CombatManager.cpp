// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "CombatManager.hpp"
#include "LevelManager.hpp"
#include "src/core/config.hpp"

void CombatManager::activate(Room &room) {
  if (room.getRoomType() != RoomType::Combat) {
    room.setCombatState(CombatState::Cleared);
    room.openAllGates();
    return;
  }

  if (room.getSpawnPoints().empty()) {
    room.setCombatState(CombatState::Cleared);
    room.openAllGates();
    return;
  }

  room.setCombatState(CombatState::Active);
  m_activeRoom = std::ref(room);
  m_combatWave = 0;
  m_gatesClosed = false;
}

bool CombatManager::tryCloseGates(Room &room, const Player &player) {
  if (room.doesPlayerOverlapGates(player))
    return false;
  room.closeAllGates();
  m_gatesClosed = true;
  return true;
}

void CombatManager::spawnWave(Room &room,
                              std::vector<std::unique_ptr<Enemy>> &enemies,
                              ResourceManager &rm) {
  const std::vector<sf::Vector2<float>> &pts = room.getSpawnPoints();
  if (pts.empty())
    return;

  m_combatWave++;
  enemies.clear();
  const sf::Texture &enemyTex = rm.getTexture(config::ENEMY_SPRITESHEET);

  for (const sf::Vector2<float> &pt : pts) {
    std::unique_ptr<Enemy> e = std::make_unique<Enemy>(enemyTex, pt);
    e->initAnimation(config::ENEMY_FRAME_W, config::ENEMY_FRAME_H,
                     config::ENEMY_FRAME_COUNT, config::ENEMY_FRAME_TIME);
    e->getSprite().setScale({config::ENEMY_SCALE, config::ENEMY_SCALE});
    enemies.push_back(std::move(e));
  }
}

void CombatManager::clearRoom(Room &room) {
  room.setCombatState(CombatState::Cleared);
  room.openAllGates();
  m_activeRoom.reset();
  m_combatWave = 0;
  m_gatesClosed = false;
}

bool CombatManager::checkWaveComplete(
    std::vector<std::unique_ptr<Enemy>> &enemies) {
  for (const auto &enemy : enemies)
    if (enemy->isEnemyAlive())
      return false;
  enemies.clear();
  return true;
}

void CombatManager::updatePlayerRoom(LevelManager &levelManager,
                                     const Player &player) {
  auto current = levelManager.findRoomAt(player.getCenter());
  if (current.has_value()) {
    Room &room = current.value().get();
    bool isNew =
        !m_previousRoom.has_value() || &m_previousRoom.value().get() != &room;
    if (isNew && room.getRoomType() == RoomType::Combat &&
        room.getCombatState() == CombatState::Inactive)
      activate(room);
    m_previousRoom = std::ref(room);
  } else {
    m_previousRoom.reset();
  }
}

void CombatManager::tryCompleteWave(
    std::vector<std::unique_ptr<Enemy>> &enemies, ResourceManager &rm) {
  if (!hasActiveRoom())
    return;
  Room &room = activeRoom();
  if (room.getCombatState() != CombatState::Active)
    return;
  if (!checkWaveComplete(enemies))
    return;
  if (currentWave() < maxWaves())
    spawnWave(room, enemies, rm);
  else {
    clearRoom(room);
    room.spawnChest(rm);
  }
}
