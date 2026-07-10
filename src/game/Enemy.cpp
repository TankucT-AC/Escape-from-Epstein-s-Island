// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Enemy.hpp"
#include "Bullet.hpp"
#include <cmath>

Enemy::Enemy(const sf::Texture &InitTexture, sf::Vector2<float> InitPos,
             float InitSpeed, float InitShootDelay, float InitHealth)
    : Entity(InitTexture, InitPos), shootTime(0.f), shootDelay(InitShootDelay),
      health(InitHealth), dead(false), speed(InitSpeed), velocity({0.f, 0.f}) {
  sf::Rect<float> bounds = sprite.getLocalBounds();
  sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

void Enemy::move(const sf::Time &dt, const sf::Vector2<float> &offset) {
  sprite.move(dt.asSeconds() * offset);
}

void Enemy::draw(sf::RenderWindow &window) const {
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

void Enemy::update(const sf::Time &dt) {
  sf::Vector2<float> offset = targetPosition - this->getPosition();
  float len = std::sqrt(offset.x * offset.x + offset.y * offset.y);
  if (len > 0)
    offset /= len;

  offset *= speed;
  this->move(dt, offset);
}

void Enemy::takeDamage(float amount) {
  health -= amount;
  if (health <= 0) {
    health = 0;
    dead = true;
  }
}

bool Enemy::isBulletCollision(const Bullet &bullet) {
  sf::Rect<float> bulletHitbox = bullet.getHitbox();
  sf::Rect<float> enemyHitbox = sprite.getGlobalBounds();

  return enemyHitbox.intersects(bulletHitbox);
}

float Enemy::getLayerY() const {
  sf::Rect<float> hitbox = this->getHitbox();
  return hitbox.top + (hitbox.height / 2.f);
}
