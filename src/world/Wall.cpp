// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Wall.hpp"
#include "src/core/config.hpp"

Wall::Wall(const sf::Texture &InitTexture, sf::Vector2<float> InitPos)
    : Entity(InitTexture, 0.f, InitPos) {
  float scale = config::TILE_SIZE / sprite.getLocalBounds().width;
  sprite.setScale({scale, scale});
}

void Wall::draw(sf::RenderWindow &window) {
  window.draw(sprite);

#if DEBUG_DRAW_COLLISIONS
  sf::Rect<float> hb = this->getHitbox();
  sf::RectangleShape debugRect({hb.width, hb.height});
  debugRect.setPosition(hb.left, hb.top);
  debugRect.setFillColor(sf::Color::Transparent);
  debugRect.setOutlineColor(sf::Color::Red);
  debugRect.setOutlineThickness(1.f);
  window.draw(debugRect);
#endif
}
