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
  DungeonData dungeon = dungeonGenerator.generateDungeon(40, 40, 4, 10, 5);

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
    auto dt = EngineClock.restart();
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

  // Удаляем пули, срок жизни которых закончился
  bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                               [](const auto &bullet) {
                                 return !bullet->isBulletAlive();
                               }),
                bullets.end());

  for (const auto &bullet : bullets) {
    if (!bullet->isBulletAlive())
      continue;
    // Наносим урон противнику
    for (const auto &enemy : enemies) {
      if (enemy->isBulletCollision(*bullet)) {
        enemy->getReceivedDamage(bullet->getDamage());
        bullet->setBulletAlive(false);
      }
    }

    // Проверка пули на то что она столкнулась со стеной
    if (room->checkCollision(*bullet))
      bullet->setBulletAlive(false);
  }

  enemies.erase(
      std::remove_if(enemies.begin(), enemies.end(),
                     [](const auto &enemy) { return !enemy->isEnemyAlive(); }),
      enemies.end());

  for (const auto &enemy : enemies) {
    enemy->update(dt, player.getPosition(), *EngineWindow);
  }

  PlayerInputState state =
      EngineInput.getPlayerInput(*EngineWindow, player, resourceManager);
  player.handlePlayer(state, resourceManager, bullets);

  EngineCamera.setCenter(player.getPosition());
}
