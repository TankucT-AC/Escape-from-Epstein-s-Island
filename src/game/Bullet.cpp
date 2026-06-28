// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Bullet.hpp"
#include "src/core/UpdateContext.hpp"
#include <SFML/System/Time.hpp>
#include <cmath>

Bullet::Bullet(const sf::Texture &InitTexture,
               const sf::Vector2<float> &InitStartPos,
               const sf::Vector2<float> &InitDirection, float InitDegree,
               float InitSpeed, float InitLifetime, float InitDamage)
    : Entity(InitTexture, InitStartPos), direction(InitDirection),
      lifetime(InitLifetime), damage(InitDamage), dead(false), speed(InitSpeed),
      velocity({0.f, 0.f}) {
  sf::Rect<float> b = sprite.getLocalBounds();
  sprite.setOrigin(b.width / 2.f, b.height / 2.f);
  sprite.setScale(0.5f, 0.5f);
  sprite.setRotation(InitDegree);
}

void Bullet::update(const UpdateContext &ctx) {
  auto offset = direction * speed;
  this->move(ctx.dt, offset);

  lifetime -= ctx.dt.asSeconds();
  if (lifetime <= 0) {
    dead = true;
    lifetime = 0;
  }
}

void Bullet::draw(sf::RenderWindow &window) {
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

void Bullet::move(const sf::Time &dt, const sf::Vector2<float> &offset) {
  sprite.move(dt.asSeconds() * offset);
}

int Bullet::getLifetime() const { return lifetime; }
float Bullet::getDamage() { return damage; }
bool Bullet::isBulletAlive() const { return !dead; }
sf::Rect<float> Bullet::getHitbox() const {
  sf::Vector2f pos = sprite.getPosition();

  float hb_width = 12.f;
  float hb_height = 4.f;

  return sf::Rect<float>(pos.x - hb_width / 2.f, pos.y - hb_height / 2.f,
                         hb_width, hb_height);
}
