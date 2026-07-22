// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Bullet.hpp"
#include <SFML/System/Time.hpp>

Bullet::Bullet(const sf::Texture &InitTexture,
               const sf::Vector2<float> &InitStartPos,
               const sf::Vector2<float> &InitDirection, float InitDegree,
               float InitSpeed, float InitLifetime, float InitDamage)
    : Entity(InitTexture, InitStartPos), direction(InitDirection),
      lifetime(InitLifetime), damage(InitDamage), dead(false), speed(InitSpeed),
      velocity({0.f, 0.f}) {
  sf::Rect<float> b = sprite.getLocalBounds();
  sprite.setOrigin(b.width / 2.f, b.height / 2.f);
  sprite.setScale(config::BULLET_SCALE, config::BULLET_SCALE);
  sprite.setRotation(InitDegree);
}

void Bullet::update(const sf::Time &dt) {
  sf::Vector2<float> offset = direction * speed;
  this->move(dt, offset);

  lifetime -= dt.asSeconds();
  if (lifetime <= 0) {
    dead = true;
    lifetime = 0;
  }
}

void Bullet::draw(sf::RenderWindow &window) const {
  window.draw(sprite);

#if DEBUG_DRAW_COLLISIONS
  sf::Rect<float> hb = this->getHitbox();
  sf::RectangleShape debugRect({hb.width, hb.height});
  debugRect.setPosition(hb.left, hb.top);
  debugRect.setFillColor(sf::Color::Transparent);
  debugRect.setOutlineColor(sf::Color::Red);
  debugRect.setOutlineThickness(config::DEBUG_OUTLINE_THICKNESS);
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
  return sf::Rect<float>(pos.x - config::BULLET_HITBOX_W / 2.f,
                         pos.y - config::BULLET_HITBOX_H / 2.f,
                         config::BULLET_HITBOX_W, config::BULLET_HITBOX_H);
}

float Bullet::getLayerY() const {
  sf::Rect<float> hitbox = this->getHitbox();
  return hitbox.top + (hitbox.height / 2.f);
}
