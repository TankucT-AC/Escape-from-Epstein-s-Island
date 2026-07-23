// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RoundManager.hpp"
#include "src/core/config.hpp"
#include "src/world/Prefabs.hpp"

RoundManager::RoundManager(DungeonGenerator &dungeonGen,
                           LevelManager &levelManager, ResourceManager &rm,
                           Player &player, CombatManager &combat,
                           PickupManager &pickups)
    : m_dungeonGen(dungeonGen), m_levelManager(levelManager), m_rm(rm),
      m_player(player), m_combat(combat), m_pickups(pickups) {}

void RoundManager::generateRound(
    std::vector<std::unique_ptr<Enemy>> &enemies,
    std::vector<std::unique_ptr<Bullet>> &bullets) {
  enemies.clear();
  bullets.clear();
  m_pickups.clear();
  m_combat.reset();
  m_combat.setMaxWaves(m_round);
  m_portalRoom.reset();
  m_portal.reset();

  m_dungeonGen.reseed();
  DungeonData dungeon = m_dungeonGen.generate();
  m_levelManager.buildFromData(dungeon, m_rm);
  m_player.setPosition(dungeon.playerSpawnPoint);
  m_player.initAnimation(config::PLAYER_FRAME_W, config::PLAYER_FRAME_H,
                         config::PLAYER_FRAME_COUNT, config::PLAYER_FRAME_TIME,
                         config::PLAYER_IDLE_ROW, config::PLAYER_RUN_ROW);
  m_player.getSprite().setScale({config::PLAYER_SCALE, config::PLAYER_SCALE});

  for (const auto &r : m_levelManager.getRooms()) {
    if (r->getRoomType() == RoomType::Portal) {
      m_portalRoom = std::ref(*r);
      break;
    }
  }
}

void RoundManager::tryAdvanceRound(
    sf::Vector2<float> worldPos, std::vector<std::unique_ptr<Enemy>> &enemies,
    std::vector<std::unique_ptr<Bullet>> &bullets) {
  if (!m_portal || !m_portal->isActive())
    return;
  if (!m_portal->getHitbox().contains(worldPos))
    return;

  m_round++;
  // m_combat.setMaxWaves(m_round);
  if (m_round > MAX_ROUNDS)
    goToPurgatory(enemies, bullets);
  else
    generateRound(enemies, bullets);
}

void RoundManager::goToPurgatory(
    std::vector<std::unique_ptr<Enemy>> &enemies,
    std::vector<std::unique_ptr<Bullet>> &bullets) {
  RoomPlacement rp;
  rp.prefabIndex = Prefabs::IDX_PURIFICATION;
  DungeonData purgData;
  purgData.rooms.push_back(rp);
  purgData.corridorWidth = 1;
  m_levelManager.buildFromData(purgData, m_rm);
  m_pickups.clear();
  enemies.clear();
  bullets.clear();
  float center = config::TILE_SIZE * Prefabs::PREFAB_SIZE / 2.f;
  m_player.setPosition({center, center});
  m_portal.reset();
}

void RoundManager::spawnPortal() {
  if (!m_portalRoom.has_value())
    return;
  m_portal =
      std::make_unique<Portal>(m_rm.getTexture(config::PORTAL_SPRITESHEET),
                               m_portalRoom.value().get().getChestPos());
  m_portal->activate();
}
