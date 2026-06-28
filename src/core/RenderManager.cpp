// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RenderManager.hpp"
#include "src/game/Bullet.hpp"
#include "src/game/Enemy.hpp"
#include "src/game/Player.hpp"
#include "src/world/Room.hpp"

RenderManager::RenderManager(sf::RenderWindow &initWindow)
    : renderWindow(initWindow) {}

void RenderManager::setCamera(const sf::View &camera) {
  renderWindow.setView(camera);
}

void RenderManager::clear() { renderWindow.clear(); }

void RenderManager::display() { renderWindow.display(); }

void RenderManager::drawRoom(Room &room) { room.draw(renderWindow); }

void RenderManager::drawPlayer(Player &player) { player.draw(renderWindow); }

void RenderManager::drawBullets(std::vector<std::unique_ptr<Bullet>> &bullets) {
  for (const auto &bullet : bullets) {
    bullet->draw(renderWindow);
  }
}

void RenderManager::drawEnemies(std::vector<std::unique_ptr<Enemy>> &enemies) {
  for (const auto &emeny : enemies) {
    emeny->draw(renderWindow);
  }
}
