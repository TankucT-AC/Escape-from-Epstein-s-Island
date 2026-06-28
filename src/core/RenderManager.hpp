// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RENDER_MANAGER_HPP
#define RENDER_MANAGER_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <memory>
#include <vector>

class Player;
class Bullet;
class Enemy;
class Room;

class RenderManager {
private:
  sf::RenderWindow &renderWindow;

public:
  explicit RenderManager(sf::RenderWindow &initWindow);
  ~RenderManager() = default;
  void setCamera(const sf::View &camera);
  void clear();

  void drawRoom(Room &room);
  void drawPlayer(Player &player);
  void drawBullets(std::vector<std::unique_ptr<Bullet>> &bullets);
  void drawEnemies(std::vector<std::unique_ptr<Enemy>> &enemies);

  void display();
};

#endif // RENDER_MANAGER_HPP
