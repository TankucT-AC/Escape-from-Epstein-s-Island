// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/core/Engine.hpp"
#include "config.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>

Engine::Engine()
    : player(resourceManager.getTexture(config::PLAYER_SPRITESHEET),
             sf::Vector2<float>{0.f, 0.f}),
      EngineVideoMode(config::GAMEBOARD_WIDTH, config::GAMEBOARD_HEIGHT),
      EngineWindow(std::make_unique<sf::RenderWindow>(EngineVideoMode,
                                                      config::GAMEBOARD_NAME)),
      EngineRender(*EngineWindow),
      dungeonGenerator(DungeonGenerator::defaultConfig()),
      roundManager(dungeonGenerator, levelManager, resourceManager, player,
                   combatManager, pickupManager) {
  player.addWeapon(std::make_unique<RangedWeapon>(
                       config::WEAPON_01_TEX, config::WEAPON_01_BULLET,
                       config::WEAPON_01_FIRE_RATE, config::WEAPON_01_DAMAGE,
                       config::WEAPON_01_BULLET_SPEED, config::WEAPON_01_SCALE),
                   resourceManager);
  roundManager.generateRound(enemies, bullets);
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
  if (roundManager.hasPortal()) {
    EngineRender.submit(roundManager.portal());
  }
  for (const auto &wp : pickupManager.getPickups())
    EngineRender.submit(*wp);
  EngineRender.submit(player);
  for (const auto &enemy : enemies)
    EngineRender.submit(*enemy);
  for (const auto &bullet : bullets)
    EngineRender.submit(*bullet);

  EngineRender.draw();
  EngineRender.display();
}

void Engine::update(const sf::Time &dt) {
  EngineInput.pollEvents(*EngineWindow, EngineEvent, EngineCamera);
  PlayerInputState state =
      EngineInput.getPlayerInput(*EngineWindow, player, resourceManager);

  player.setMousePos(state.mousePos);
  player.update(dt);
  player.moveShootTime(dt);

  if (roundManager.hasPortal())
    roundManager.portal().update(dt);

  for (const auto &bullet : bullets)
    bullet->update(dt);
  for (const auto &enemy : enemies) {
    enemy->setTargetPosition(player.getPosition());
    enemy->update(dt);
  }
  EnginePhysics.moveEnemies(dt, enemies, levelManager);

  EnginePhysics.handleCollisions(bullets, enemies, levelManager);
  EnginePhysics.handleCollisions(dt, player, levelManager);
  EnginePhysics.cleanup(bullets, enemies, levelManager);

  player.syncWeaponTransform();
  player.handlePlayer(state, resourceManager, bullets);

  if (state.weaponSlot >= 0)
    player.switchWeapon(state.weaponSlot, resourceManager);

  combatManager.updatePlayerRoom(levelManager, player);

  if (combatManager.isSpawnPending()) {
    Room &activeRoom = combatManager.activeRoom();
    if (combatManager.tryCloseGates(activeRoom, player))
      combatManager.spawnWave(activeRoom, enemies, resourceManager);
  } else {
    combatManager.tryCompleteWave(enemies, resourceManager);
  }

  if (!roundManager.hasPortal() && levelManager.allCombatRoomsCleared())
    roundManager.spawnPortal();

  if (state.wantToInteract) {
    pickupManager.trySwapWeapon(player, resourceManager, state.mousePos);
    pickupManager.tryOpenChest(levelManager, resourceManager, state.mousePos,
                               roundManager.round());
    roundManager.tryAdvanceRound(state.mousePos, enemies, bullets);
  }

  EngineCamera.setCenter(player.getPosition());
}
