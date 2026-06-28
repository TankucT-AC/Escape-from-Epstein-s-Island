// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PhysicsManager.hpp"
#include "src/game/Bullet.hpp"
#include "src/game/Enemy.hpp"
#include "src/game/Player.hpp"
#include "src/world/Room.hpp"

void PhysicsManager::handleCollisions(
    std::vector<std::unique_ptr<Bullet>> &bullets,
    std::vector<std::unique_ptr<Enemy>> &enemies, Room &room) {
  for (const auto &bullet : bullets) {
    // Наносим урон противнику
    for (const auto &enemy : enemies) {
      if (enemy->isBulletCollision(*bullet)) {
        enemy->takeDamage(bullet->getDamage());
        bullet->setBulletAlive(false);
      }
    }

    // Проверка пули на то что она столкнулась со стеной
    if (room.checkCollision(*bullet))
      bullet->setBulletAlive(false);
  }
}

void PhysicsManager::cleanup(std::vector<std::unique_ptr<Bullet>> &bullets,
                             std::vector<std::unique_ptr<Enemy>> &enemies,
                             Room &room) {
  // Удаляем пули, срок жизни которых закончился
  bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                               [](const auto &bullet) {
                                 return !bullet->isBulletAlive();
                               }),
                bullets.end());

  // Удаляем врагов, срок жизни которых закончился
  enemies.erase(
      std::remove_if(enemies.begin(), enemies.end(),
                     [](const auto &enemy) { return !enemy->isEnemyAlive(); }),
      enemies.end());
}

void PhysicsManager::checkCollisions(const sf::Time &dt, Player &player,
                                     Room &room) {
  sf::Vector2<float> oldPos = player.getPosition();

  player.move(dt, sf::Vector2<float>{player.getVelocity().x, 0.f});
  if (room.checkCollision(player)) {
    player.setPosition(sf::Vector2<float>{oldPos.x, player.getPosition().y});
  }

  player.move(dt, sf::Vector2<float>{0.f, player.getVelocity().y});
  if (room.checkCollision(player)) {
    player.setPosition(sf::Vector2<float>{player.getPosition().x, oldPos.y});
  }
}
