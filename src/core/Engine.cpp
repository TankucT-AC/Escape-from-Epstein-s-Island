// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/core/Engine.hpp"
#include "UpdateContext.hpp"
#include "config.hpp"
#include "src/core/InputManager.hpp"
#include "src/game/Bullet.hpp"
#include <cmath>
#include <memory>

Engine::Engine()
    : player(resourceManager.getTexture(config::PLAYER_TEXTURE),
             sf::Vector2<float>{0.f, 0.f}),
      EngineVideoMode(config::GAMEBOARD_WIDTH, config::GAMEBOARD_HEIGHT),
      EngineWindow(std::make_unique<sf::RenderWindow>(EngineVideoMode,
                                                      config::GAMEBOARD_NAME)),
      EngineRender(*EngineWindow) {
  DungeonData dungeon = dungeonGenerator.generateDungeon(60, 60, 4, 15, 7);

  room = std::make_unique<Room>(dungeon.grid, sf::Vector2<float>(0.f, 0.f),
                                resourceManager);

  player.setPosition(dungeon.playerSpawnPoint);

  enemies.push_back(std::make_unique<Enemy>(
      resourceManager.getTexture(config::ENEMY_TEXTURE),
      dungeon.playerSpawnPoint +
          sf::Vector2<float>(config::TILE_SIZE * 2.f, 0.f)));

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

  EngineRender.drawRoom(*room);
  EngineRender.drawEntity(player);
  EngineRender.drawEnemies(enemies);
  EngineRender.drawBullets(bullets);

  EngineRender.display();
}

void Engine::update(const sf::Time &dt) {
  EngineInput.pollEvents(*EngineWindow, EngineEvent, EngineCamera);

  player.update(UpdateContext(dt, *EngineWindow, *room));
  player.moveShootTime(dt);
  for (const auto &bullet : bullets) {
    bullet->update(UpdateContext(dt, *EngineWindow, *room));
  }
  for (const auto &enemy : enemies) {
    enemy->update(dt, player.getPosition(), *EngineWindow);
  }

  EnginePhysics.handleCollisions(bullets, enemies, *room);
  EnginePhysics.cleanup(bullets, enemies, *room);
  EnginePhysics.checkCollisions(dt, player, *room);

  PlayerInputState state =
      EngineInput.getPlayerInput(*EngineWindow, player, resourceManager);
  player.handlePlayer(state, resourceManager, bullets);

  EngineCamera.setCenter(player.getPosition());
}
