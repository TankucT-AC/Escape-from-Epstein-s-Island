// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Enemy.hpp"
#include "Bullet.hpp"
#include "src/core/config.hpp"
#include <cmath>

Enemy::Enemy(const sf::Texture &InitTexture, sf::Vector2<float> InitPos,
             float InitSpeed, float InitHealth)
    : Entity(InitTexture, InitPos), health(InitHealth), dead(false),
      speed(InitSpeed), velocity({0.f, 0.f}) {}

void Enemy::move(const sf::Time &dt, const sf::Vector2<float> &offset) {
  sprite.move(dt.asSeconds() * offset);
}

void Enemy::draw(sf::RenderWindow &window) const {
  window.draw(sprite);

#if DEBUG_DRAW_COLLISIONS
  auto hb = getHitbox();
  sf::RectangleShape debugRect({hb.width, hb.height});
  debugRect.setPosition(hb.left, hb.top);
  debugRect.setFillColor(sf::Color::Transparent);
  debugRect.setOutlineColor(sf::Color::Red);
  debugRect.setOutlineThickness(config::DEBUG_OUTLINE_THICKNESS);
  window.draw(debugRect);
#endif
}

void Enemy::update(const sf::Time &dt) {
  sf::Vector2<float> offset = targetPosition - getPosition();
  float len = std::sqrt(offset.x * offset.x + offset.y * offset.y);
  bool moving = len > 0;
  if (moving)
    offset /= len;

  offset *= speed;
  velocity = offset;
  m_animator.update(dt.asSeconds(), moving, sprite);
}

void Enemy::takeDamage(float amount) {
  health -= amount;
  if (health <= 0) {
    health = 0;
    dead = true;
  }
}

bool Enemy::isBulletCollision(const Bullet &bullet) {
  return sprite.getGlobalBounds().intersects(bullet.getHitbox());
}

float Enemy::getLayerY() const {
  auto hb = getHitbox();
  return hb.top + hb.height / 2.f;
}