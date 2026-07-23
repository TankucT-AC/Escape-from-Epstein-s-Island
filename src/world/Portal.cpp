// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Portal.hpp"
#include "src/core/config.hpp"

Portal::Portal(const sf::Texture &tex, sf::Vector2<float> pos)
    : Entity(tex, pos), m_active(false) {
  sprite.setScale(config::PORTAL_INACTIVE_SCALE, config::PORTAL_INACTIVE_SCALE);
}

void Portal::activate() {
  m_active = true;
  m_animator.init(config::PORTAL_FRAME_W, config::PORTAL_FRAME_H,
                  config::PORTAL_FRAME_COUNT, config::PORTAL_FRAME_TIME);
  sprite.setTextureRect(
      {0, 0, config::PORTAL_FRAME_W, config::PORTAL_FRAME_H});
  sprite.setOrigin(config::PORTAL_FRAME_W / 2.f, config::PORTAL_FRAME_H / 2.f);
  sprite.setScale(
      {config::PORTAL_SCALE * config::TILE_SIZE /
           static_cast<float>(config::PORTAL_FRAME_W),
       config::PORTAL_SCALE * config::TILE_SIZE /
           static_cast<float>(config::PORTAL_FRAME_W)});
}

void Portal::update(const sf::Time &dt) {
  if (m_active)
    m_animator.update(dt.asSeconds(), true, sprite);
}

void Portal::draw(sf::RenderWindow &window) const { window.draw(sprite); }

float Portal::getLayerY() const {
  auto hb = sprite.getGlobalBounds();
  return hb.top + hb.height / 2.f;
}
