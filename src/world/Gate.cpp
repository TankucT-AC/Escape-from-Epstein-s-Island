// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Gate.hpp"
#include "src/core/config.hpp"
#include <SFML/Graphics/Rect.hpp>

Gate::Gate(const sf::Texture &openTex, const sf::Texture &closedTex,
           sf::Vector2<float> pos)
    : Entity(openTex, pos), m_state(State::Open), m_openTex(openTex),
      m_closedTex(closedTex) {
  float scale = config::TILE_SIZE / sprite.getLocalBounds().width;
  sprite.setScale({scale, scale});
}

void Gate::setOpen() {
  m_state = State::Open;
  sprite.setTexture(m_openTex);
  float scale = config::TILE_SIZE / sprite.getLocalBounds().width;
  sprite.setScale({scale, scale});
}

void Gate::setClosed() {
  m_state = State::Closed;
  sprite.setTexture(m_closedTex);
  float scale = config::TILE_SIZE / sprite.getLocalBounds().width;
  sprite.setScale({scale, scale});
}

void Gate::draw(sf::RenderWindow &window) const { window.draw(sprite); }

float Gate::getLayerY() const {
  // sf::Rect<float> hb = sprite.getGlobalBounds();
  // return hb.top + hb.height / 2.f;
  return -1000.f;
}
