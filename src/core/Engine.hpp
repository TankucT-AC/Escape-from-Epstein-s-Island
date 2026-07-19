// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "InputManager.hpp"
#include "PhysicsManager.hpp"
#include "RenderManager.hpp"
#include "ResourceManager.hpp"
#include "src/game/Bullet.hpp"
#include "src/game/Enemy.hpp"
#include "src/game/Player.hpp"
#include "src/world/DungeonGenerator.hpp"
#include "src/world/LevelManager.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <memory>
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

public:
  Engine();

  void run();

  void render();

  void update(const sf::Time &dt);
};

#endif
