// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Chest.hpp"
#include "src/core/config.hpp"

Chest::Chest(const sf::Texture &closedTex, const sf::Texture &openTex,
             sf::Vector2<float> pos)
    : Entity(closedTex, pos), m_state(State::Closed), m_closedTex(closedTex),
      m_openTex(openTex) {
  float scale = config::TILE_SIZE / sprite.getLocalBounds().width;
  sprite.setScale({scale, scale});
}

void Chest::open() {
  if (m_state == State::Open)
    return;
  m_state = State::Open;
  sprite.setTexture(m_openTex);
  float scale = config::TILE_SIZE / sprite.getLocalBounds().width;
  sprite.setScale({scale, scale});
}

void Chest::draw(sf::RenderWindow &window) const { window.draw(sprite); }

float Chest::getLayerY() const {
  // sf::Rect<float> hb = sprite.getGlobalBounds();
  // return hb.top + hb.height / 2.f;
  return -1000.f;
}
