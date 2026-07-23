// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PhysicsManager.hpp"
#include "LevelManager.hpp"
#include "src/game/Bullet.hpp"
#include "src/game/Enemy.hpp"
#include "src/game/Player.hpp"

void PhysicsManager::handleCollisions(
    std::vector<std::unique_ptr<Bullet>> &bullets,
    std::vector<std::unique_ptr<Enemy>> &enemies, LevelManager &levelManager) {
  for (const auto &bullet : bullets) {
    for (const auto &enemy : enemies) {
      if (enemy->isBulletCollision(*bullet)) {
        enemy->takeDamage(bullet->getDamage());
        bullet->setBulletAlive(false);
      }
    }

    if (levelManager.checkCollision(*bullet))
      bullet->setBulletAlive(false);
  }
}

void PhysicsManager::cleanup(std::vector<std::unique_ptr<Bullet>> &bullets,
                             std::vector<std::unique_ptr<Enemy>> &enemies,
                             LevelManager &levelManager) {
  bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                               [](const auto &bullet) {
                                 return !bullet->isBulletAlive();
                               }),
                bullets.end());

  enemies.erase(
      std::remove_if(enemies.begin(), enemies.end(),
                     [](const auto &enemy) { return !enemy->isEnemyAlive(); }),
      enemies.end());
}

void PhysicsManager::handleCollisions(const sf::Time &dt, Player &player,
                                      LevelManager &levelManager) {
  sf::Vector2<float> oldPos = player.getPosition();

  player.move(dt, sf::Vector2<float>{player.getVelocity().x, 0.f});
  if (levelManager.checkCollision(player)) {
    player.setPosition(sf::Vector2<float>{oldPos.x, player.getPosition().y});
  }

  player.move(dt, sf::Vector2<float>{0.f, player.getVelocity().y});
  if (levelManager.checkCollision(player)) {
    player.setPosition(sf::Vector2<float>{player.getPosition().x, oldPos.y});
  }
}

void PhysicsManager::moveEnemies(const sf::Time &dt,
                                 std::vector<std::unique_ptr<Enemy>> &enemies,
                                 LevelManager &levelManager) {
  for (auto &enemy : enemies) {
    sf::Vector2<float> oldPos = enemy->getPosition();
    enemy->move(dt, enemy->getVelocity());
    if (levelManager.checkCollision(*enemy))
      enemy->setPosition(oldPos);
  }
}
