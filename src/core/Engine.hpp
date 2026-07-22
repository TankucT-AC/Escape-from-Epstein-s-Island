// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "CombatManager.hpp"
#include "InputManager.hpp"
#include "LevelManager.hpp"
#include "PhysicsManager.hpp"
#include "RenderManager.hpp"
#include "ResourceManager.hpp"
#include "src/game/Bullet.hpp"
#include "src/game/Enemy.hpp"
#include "src/game/Player.hpp"
#include "src/game/Weapon.hpp"
#include "src/game/WeaponPickup.hpp"
#include "src/world/DungeonGenerator.hpp"
#include "src/world/Portal.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

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

  std::optional<std::reference_wrapper<Room>> m_previousRoom;
  std::optional<std::reference_wrapper<Room>> m_portalRoom;

  int m_round = 1;
  static const int MAX_ROUNDS = 3;
  std::unique_ptr<Portal> m_portal;

  std::vector<std::unique_ptr<WeaponPickup>> m_weaponPickups;

  void generateRound();
  void tryActivateCombat(Room &room);
  void tryCompleteCombatWave();
  void spawnPortalInRoom(Room &room);
  std::unique_ptr<WeaponPickup> swapWeapon(WeaponPickup &pickup);
  void tryOpenChest(sf::Vector2<float> worldPos);
  void tryAdvanceRound(sf::Vector2<float> worldPos);
  void goToPurgatory();

public:
  Engine();
  void run();
  void render();
  void update(const sf::Time &dt);
};

#endif