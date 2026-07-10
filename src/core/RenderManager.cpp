// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RenderManager.hpp"
#include "src/game/Entity.hpp"
#include <functional>

RenderManager::RenderManager(sf::RenderWindow &initWindow)
    : renderWindow(initWindow) {}

void RenderManager::setCamera(const sf::View &camera) {
  renderWindow.setView(camera);
}

void RenderManager::clear() { renderWindow.clear(); }
void RenderManager::display() { renderWindow.display(); }

void RenderManager::submit(std::reference_wrapper<IDrawAble> object) {
  renderQueue.push_back(object);
}

void RenderManager::draw() {
  std::sort(renderQueue.begin(), renderQueue.end(),
            [](const IDrawAble &a, const IDrawAble &b) {
              return a.getLayerY() < b.getLayerY();
            });

  for (IDrawAble &object : renderQueue) {
    object.draw(renderWindow);
  }

  renderQueue.clear();
}
