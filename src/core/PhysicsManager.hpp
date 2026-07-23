// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PHYSICS_MANAGER_HPP
#define PHYSICS_MANAGER_HPP

#include <SFML/System/Clock.hpp>
#include <memory>
#include <vector>

class Player;
class LevelManager;
class Enemy;
class Bullet;

class PhysicsManager {
public:
  void handleCollisions(std::vector<std::unique_ptr<Bullet>> &bullets,
                        std::vector<std::unique_ptr<Enemy>> &enemies,
                        LevelManager &levelManager);

  void cleanup(std::vector<std::unique_ptr<Bullet>> &bullets,
               std::vector<std::unique_ptr<Enemy>> &enemies,
               LevelManager &levelManager);

  void handleCollisions(const sf::Time &dt, Player &player,
                        LevelManager &levelManager);

  /**
   * @brief Двигает всех врагов и разрешает коллизии со стенами.
   * @param dt Дельта времени.
   * @param enemies Вектор врагов (изменяются позиции).
   * @param levelManager Менеджер уровня для проверки коллизий.
   */
  static void moveEnemies(const sf::Time &dt,
                          std::vector<std::unique_ptr<Enemy>> &enemies,
                          LevelManager &levelManager);
};

#endif // PHYSICS_MANAGER_HPP
