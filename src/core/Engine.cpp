// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/core/Engine.hpp"
#include "config.hpp"
#include "src/core/InputManager.hpp"
#include "src/game/Bullet.hpp"
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

  room->submitRender(EngineRender);
  EngineRender.submit(player);
  for (const auto &enemy : enemies) {
    EngineRender.submit(*enemy);
  }
  for (const auto &bullet : bullets) {
    EngineRender.submit(*bullet);
  }

  EngineRender.draw();
  EngineRender.display();
}

void Engine::update(const sf::Time &dt) {
  EngineInput.pollEvents(*EngineWindow, EngineEvent, EngineCamera);

  sf::Vector2<int> mousePos = sf::Mouse::getPosition(*EngineWindow);
  sf::Vector2<float> worldMousePos = EngineWindow->mapPixelToCoords(mousePos);
  player.setMousePos(worldMousePos);
  player.update(dt);
  player.moveShootTime(dt);

  for (const auto &bullet : bullets) {
    bullet->update(dt);
  }
  for (const auto &enemy : enemies) {
    enemy->setTargetPosition(player.getPosition());
    enemy->update(dt);
  }

  EnginePhysics.handleCollisions(bullets, enemies, *room);
  EnginePhysics.cleanup(bullets, enemies, *room);
  EnginePhysics.checkCollisions(dt, player, *room);

  PlayerInputState state =
      EngineInput.getPlayerInput(*EngineWindow, player, resourceManager);
  player.handlePlayer(state, resourceManager, bullets);

  EngineCamera.setCenter(player.getPosition());
}
