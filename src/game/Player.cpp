// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Player.hpp"
#include "src/core/InputManager.hpp"
#include "src/core/ResourceManager.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include <functional>
#include <optional>

Player::Player(const sf::Texture &InitTexture, sf::Vector2<float> InitPos,
               float InitSpeed, float InitShootDelay)
    : Entity(InitTexture, InitPos), m_shootTimer(0.f),
      m_shootDelay(InitShootDelay), m_velocity{0.f, 0.f}, m_speed(InitSpeed) {}

void Player::move(const sf::Time &dt, const sf::Vector2<float> &offset) {
  sprite.move(dt.asSeconds() * offset);
}

void Player::draw(sf::RenderWindow &window) const {
  window.draw(sprite);
  if (m_activeWeapon >= 0 && m_weaponSprite.getTexture()) {
    window.draw(m_weaponSprite);

#if DEBUG_DRAW_COLLISIONS
    auto gb = m_weaponSprite.getGlobalBounds();
    sf::RectangleShape debugWeapon({gb.width, gb.height});
    debugWeapon.setPosition(gb.left, gb.top);
    debugWeapon.setFillColor(sf::Color::Transparent);
    debugWeapon.setOutlineColor(sf::Color::Yellow);
    debugWeapon.setOutlineThickness(config::DEBUG_OUTLINE_THICKNESS);
    window.draw(debugWeapon);
#endif
  }

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

void Player::setMousePos(const sf::Vector2<float> &pos) { m_mousePos = pos; }

void Player::update(const sf::Time &dt) {
  auto offset = sf::Vector2<float>(0.f, 0.f);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    offset.y = -m_speed;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    offset.y = m_speed;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    offset.x = -m_speed;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    offset.x = m_speed;

  float len = std::sqrt(offset.x * offset.x + offset.y * offset.y);
  bool moving = len > 0;
  if (moving)
    offset /= len;

  m_velocity = offset * m_speed;
  m_animator.update(dt.asSeconds(), moving, sprite);

  bool facingLeft = (m_mousePos.x < sprite.getPosition().x);
  sprite.setScale(facingLeft ? -config::PLAYER_SCALE : config::PLAYER_SCALE,
                  config::PLAYER_SCALE);
}

void Player::syncWeaponTransform() {
  sf::Vector2<float> playerPos = sprite.getPosition();
  sf::Vector2<float> dir = m_mousePos - playerPos;
  auto radians = std::atan2(dir.y, dir.x);
  auto degrees = radians * 180.f / config::PI;
  bool facingLeft = (m_mousePos.x < playerPos.x);

  m_weaponSprite.setPosition(playerPos.x,
                             playerPos.y + config::PLAYER_WEAPON_Y_OFFSET);
  m_weaponSprite.setRotation(degrees);
  float sx = std::abs(m_weaponSprite.getScale().x);
  float sy = std::abs(m_weaponSprite.getScale().y);
  m_weaponSprite.setScale(sx, facingLeft ? -sy : sy);
}

void Player::moveShootTime(const sf::Time &dt) {
  m_shootTimer -= dt.asSeconds();
}
bool Player::isShootTime() const { return m_shootTimer <= 0; }
void Player::cooldown() { m_shootTimer = m_shootDelay; }

float Player::getLayerY() const {
  auto hb = getHitbox();
  return hb.top + hb.height / 2.f;
}

sf::FloatRect Player::getHitbox() const {
  auto pos = sprite.getPosition();
  return {pos.x - config::PLAYER_HITBOX_HALF_W,
          pos.y - config::PLAYER_HITBOX_HALF_H + config::PLAYER_HITBOX_Y_OFFSET,
          config::PLAYER_HITBOX_W, config::PLAYER_HITBOX_H};
}

// ─── Оружие ───

void Player::updateWeaponSprite(ResourceManager &rm) {
  if (m_activeWeapon < 0 ||
      m_activeWeapon >= static_cast<int>(m_weapons.size()))
    return;
  const Weapon *w = m_weapons[m_activeWeapon].get();
  auto &tex = rm.getTexture(w->getWeaponTexPath());
  // resetRect=true — иначе textureRect остаётся от предыдущего оружия
  // и текстура нового обрезается (размеры пресетов различаются)
  m_weaponSprite.setTexture(tex, true);
  auto b = m_weaponSprite.getLocalBounds();
  m_weaponSprite.setOrigin(b.width / 2.f, b.height / 2.f);
  float baseScale = w->getWeaponScale();
  m_weaponSprite.setScale({baseScale, baseScale});
}

void Player::addWeapon(std::unique_ptr<Weapon> w, ResourceManager &rm) {
  m_weapons.push_back(std::move(w));
  if (m_weapons.size() == 1) {
    m_activeWeapon = 0;
    updateWeaponSprite(rm);
  }
}

void Player::switchWeapon(int slot, ResourceManager &rm) {
  if (slot < 0 || slot >= static_cast<int>(m_weapons.size()))
    return;
  m_activeWeapon = slot;
  updateWeaponSprite(rm);
}

std::unique_ptr<Weapon> Player::removeWeapon(int slot) {
  if (slot < 0 || slot >= static_cast<int>(m_weapons.size()))
    return nullptr;
  auto w = std::move(m_weapons[slot]);
  m_weapons.erase(m_weapons.begin() + slot);
  if (m_activeWeapon >= static_cast<int>(m_weapons.size()))
    m_activeWeapon = m_weapons.empty() ? -1 : 0;
  return w;
}

const std::optional<std::reference_wrapper<Weapon>>
Player::getActiveWeapon() const {
  if (m_activeWeapon >= 0 &&
      m_activeWeapon < static_cast<int>(m_weapons.size()))
    return *m_weapons[m_activeWeapon];
  return std::nullopt;
}

void Player::handlePlayer(const PlayerInputState &input, ResourceManager &rm,
                          std::vector<std::unique_ptr<Bullet>> &bullets) {
  if (input.wantToShoot && isShootTime()) {
    const std::optional<std::reference_wrapper<Weapon>> w = getActiveWeapon();
    if (w.has_value()) {
      // Дуло: от позиции оружия (а не центра игрока) вдоль направления прицела
      // на половину отмасштабированной ширины спрайта — кончик ствола.
      sf::Vector2<float> weaponPos = {
          getPosition().x, getPosition().y + config::PLAYER_WEAPON_Y_OFFSET};
      sf::Vector2<float> dir = input.mousePos - weaponPos;
      float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
      if (len > 0)
        dir /= len;

      float barrelLen = m_weaponSprite.getLocalBounds().width *
                        std::abs(m_weaponSprite.getScale().x) * 0.5f;
      sf::Vector2<float> barrelPos = weaponPos + dir * barrelLen;

      // Неконстантный fire (Weapon не может быть const из-за виртуального fire)
      // Кастуем — оружие физически не меняется от fire
      const_cast<Weapon &>(w.value().get()).fire(barrelPos, dir, bullets, rm);
      m_shootDelay = w.value().get().getFireRate();
      cooldown();
    }
  }
}
