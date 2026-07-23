// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "CombatManager.hpp"
#include "InputManager.hpp"
#include "LevelManager.hpp"
#include "PhysicsManager.hpp"
#include "PickupManager.hpp"
#include "RenderManager.hpp"
#include "ResourceManager.hpp"
#include "RoundManager.hpp"
#include "src/game/Bullet.hpp"
#include "src/game/Enemy.hpp"
#include "src/game/Player.hpp"
#include "src/game/Weapon.hpp"
#include "src/world/DungeonGenerator.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <memory>
#include <vector>

/**
 * @brief Главный координатор игры (точка входа в game loop).
 *
 * Владеет окном, менеджерами, игроком, врагами и пулями.
 * Содержит только три публичных метода: run(), render(), update().
 * Вся остальная логика делегирована менеджерам:
 *   CombatManager — бои и волны,
 *   RoundManager   — генерация и прогрессия раундов/портал,
 *   PickupManager  — оружие/сундуки,
 *   PhysicsManager — коллизии и движение,
 *   InputManager   — обработка ввода.
 */
class Engine {
private:
  sf::VideoMode EngineVideoMode;
  std::unique_ptr<sf::RenderWindow> EngineWindow;
  sf::Event EngineEvent;
  sf::Clock EngineClock;
  sf::View EngineCamera;
  ResourceManager resourceManager;
  InputManager EngineInput;
  RenderManager EngineRender;
  PhysicsManager EnginePhysics;
  Player player;
  std::vector<std::unique_ptr<Enemy>> enemies;
  std::vector<std::unique_ptr<Bullet>> bullets;
  DungeonGenerator dungeonGenerator;
  LevelManager levelManager;
  CombatManager combatManager;
  PickupManager pickupManager;
  RoundManager roundManager;

public:
  /** @brief Инициализирует окно, менеджеры, игрока и первый раунд. */
  Engine();

  /** @brief Главный игровой цикл. */
  void run();

  /** @brief Отрисовка всех сущностей и объектов. */
  void render();

  /** @brief Обновление игровой логики за один кадр. */
  void update(const sf::Time &dt);
};

#endif
