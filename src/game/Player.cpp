// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Player.hpp"
#include "src/core/InputManager.hpp"
#include "src/core/ResourceManager.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>

Player::Player(const sf::Texture &InitTexture, sf::Vector2<float> InitPos,
               float InitSpeed, float InitShootDelay)
    : Entity(InitTexture, InitPos), ShootTime(0.f), ShootDelay(InitShootDelay),
      velocity{0.f, 0.f}, speed(InitSpeed) {}

void Player::move(const sf::Time &dt, const sf::Vector2<float> &offset) {
  sprite.move(dt.asSeconds() * offset);
}

void Player::draw(sf::RenderWindow &window) const {
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

void Player::setMousePos(const sf::Vector2<float> &worldMousePos) {
  mousePos = worldMousePos;
}

void Player::update(const sf::Time &dt) {
  // перемещение игрока в пространстве
  auto offset = sf::Vector2<float>(0.f, 0.f);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    offset.y = -speed;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    offset.y = speed;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    offset.x = -speed;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    offset.x = speed;

  float length = std::sqrt(offset.x * offset.x + offset.y * offset.y);
  if (length > 0) {
    offset /= length;
  }

  velocity = offset * speed;

  // вращение игрока в зависимости от позиции мышки
  sf::Vector2<float> playerPos = sprite.getPosition();
  sf::Vector2<float> direction = mousePos - playerPos;

  auto radians = static_cast<float>(std::atan2(direction.y, direction.x));

  auto degrees = radians * 180.f / config::PI;
  sprite.setRotation(degrees);
}

void Player::moveShootTime(const sf::Time &dt) { ShootTime -= dt.asSeconds(); }

bool Player::isShootTime() const { return ShootTime <= 0; }

sf::Rect<float> Player::getHitbox() const {
  sf::Vector2<float> pos = sprite.getPosition();

  float hb_width = 24.f;
  float hb_height = 16.f;
  float offsetY = 20.f;

  return sf::Rect<float>(pos.x - hb_width / 2.f,
                         pos.y - hb_height / 2.f + offsetY, hb_width,
                         hb_height);
}

float Player::getLayerY() const {
  sf::Rect<float> hitbox = this->getHitbox();
  return hitbox.top + (hitbox.height / 2.f);
}

void Player::cooldown() { ShootTime = ShootDelay; }

void Player::handlePlayer(const PlayerInputState &input, ResourceManager &rm,
                          std::vector<std::unique_ptr<Bullet>> &bullets) {
  if (input.wantToShoot && this->isShootTime()) {
    // Задаем вектор для пули
    sf::Vector2<float> dir = input.mousePos - this->getPosition();

    // Нормализуем вектор
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0)
      dir /= len;
    auto radians = static_cast<float>(std::atan2(dir.y, dir.x));
    auto degrees = radians * 180.f / config::PI;

    // Добавляем пулю в массив
    bullets.push_back(
        std::make_unique<Bullet>(rm.getTexture(config::BULLET_PLAYER_TEXTURE),
                                 this->getPosition(), dir, degrees));
    this->cooldown();
  }
}
