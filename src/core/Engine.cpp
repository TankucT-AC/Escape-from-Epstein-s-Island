// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/core/Engine.hpp"
#include "config.hpp"
#include "src/core/InputManager.hpp"
#include "src/game/Bullet.hpp"
#include "src/world/Chest.hpp"
#include "src/world/DungeonGenerator.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include <functional>
#include <memory>
#include <optional>

Engine::Engine()
    : player(resourceManager.getTexture(config::PLAYER_TEXTURE),
             sf::Vector2<float>{0.f, 0.f}),
      EngineVideoMode(config::GAMEBOARD_WIDTH, config::GAMEBOARD_HEIGHT),
      EngineWindow(std::make_unique<sf::RenderWindow>(EngineVideoMode,
                                                      config::GAMEBOARD_NAME)),
      EngineRender(*EngineWindow),
      dungeonGenerator(DungeonGenerator::defaultConfig()) {
  DungeonData dungeon = dungeonGenerator.generate();
  levelManager.buildFromData(dungeon, resourceManager);
  player.setPosition(dungeon.playerSpawnPoint);
  EngineCamera.setCenter(player.getPosition());
}

void Engine::run() {
  while (EngineWindow->isOpen()) {
    sf::Time dt = EngineClock.restart();
    this->update(dt);
    this->render();
  }
}

void Engine::render() {
  EngineRender.setCamera(EngineCamera);
  EngineRender.clear();

  levelManager.submitRender(EngineRender);
  EngineRender.submit(player);
  for (const auto &enemy : enemies)
    EngineRender.submit(*enemy);
  for (const auto &bullet : bullets)
    EngineRender.submit(*bullet);

  EngineRender.draw();
  EngineRender.display();
}

void Engine::activateCombatRoom(Room &room) {
  room.setCombatState(CombatState::Active);
  m_activeCombatRoom = std::ref(room);
  m_combatWave = 0;

  if (room.doesPlayerOverlapGates(player)) {
    return;
  }

  room.closeAllGates();
  spawnCombatWave(room);
}

void Engine::spawnCombatWave(Room &room) {
  const std::vector<sf::Vector2<float>> &pts = room.getSpawnPoints();
  if (pts.empty())
    return;

  m_combatWave++;
  const sf::Texture &enemyTex =
      resourceManager.getTexture(config::ENEMY_TEXTURE);

  for (const sf::Vector2<float> &pt : pts)
    enemies.push_back(std::make_unique<Enemy>(enemyTex, pt));
}

void Engine::clearCombatRoom(Room &room) {
  room.setCombatState(CombatState::Cleared);
  room.openAllGates();
  room.spawnChest(resourceManager);
  m_activeCombatRoom.reset();
  m_combatWave = 0;
}

void Engine::update(const sf::Time &dt) {
  EngineInput.pollEvents(*EngineWindow, EngineEvent, EngineCamera);

  PlayerInputState state =
      EngineInput.getPlayerInput(*EngineWindow, player, resourceManager);

  player.setMousePos(state.mousePos);
  player.update(dt);
  player.moveShootTime(dt);

  for (const auto &bullet : bullets)
    bullet->update(dt);
  for (const auto &enemy : enemies) {
    enemy->setTargetPosition(player.getPosition());
    enemy->update(dt);
  }

  for (auto &enemy : enemies) {
    sf::Vector2<float> oldPos = enemy->getPosition();
    enemy->move(dt, enemy->getVelocity());
    if (levelManager.checkCollision(*enemy)) {
      enemy->setPosition(oldPos);
    }
  }

  EnginePhysics.handleCollisions(bullets, enemies, levelManager);
  EnginePhysics.cleanup(bullets, enemies, levelManager);
  EnginePhysics.checkCollisions(dt, player, levelManager);

  player.handlePlayer(state, resourceManager, bullets);

  // ── Комнаты ──
  std::optional<std::reference_wrapper<Room>> currentRoomOpt =
      levelManager.findRoomAt(player.getCenter());

  if (currentRoomOpt.has_value()) {
    Room &currentRoom = currentRoomOpt.value().get();
    bool isNewRoom = !m_previousRoom.has_value() ||
                     &m_previousRoom.value().get() != &currentRoom;

    if (isNewRoom && currentRoom.getRoomType() == RoomType::Combat &&
        currentRoom.getCombatState() == CombatState::Inactive) {
      activateCombatRoom(currentRoom);
    }
    m_previousRoom = std::ref(currentRoom);
  } else {
    m_previousRoom.reset();
  }

  // Отложенное закрытие ворот
  if (m_activeCombatRoom.has_value()) {
    Room &activeRoom = m_activeCombatRoom.value().get();
    if (activeRoom.getCombatState() == CombatState::Active &&
        !activeRoom.doesPlayerOverlapGates(player) && m_combatWave == 0) {
      activeRoom.closeAllGates();
      spawnCombatWave(activeRoom);
    }
  }

  // Завершение волны
  if (m_activeCombatRoom.has_value() && m_combatWave > 0) {
    Room &activeRoom = m_activeCombatRoom.value().get();
    if (activeRoom.getCombatState() == CombatState::Active) {
      bool allDead = true;
      for (const auto &enemy : enemies)
        if (enemy->isEnemyAlive()) {
          allDead = false;
          break;
        }

      if (allDead) {
        enemies.clear();
        if (m_combatWave < 3)
          spawnCombatWave(activeRoom);
        else
          clearCombatRoom(activeRoom);
      }
    }
  }

  // Взаимодействие с сундуком
  if (state.wantToInteract) {
    std::optional<std::reference_wrapper<Chest>> chestOpt =
        levelManager.findChestAt(state.mousePos);
    if (chestOpt.has_value())
      chestOpt.value().get().open();
  }

  EngineCamera.setCenter(player.getPosition());
}
