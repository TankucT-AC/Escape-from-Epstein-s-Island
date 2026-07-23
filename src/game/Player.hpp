// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "AnimatedSprite.hpp"
#include "Bullet.hpp"
#include "Weapon.hpp"
#include "src/core/ResourceManager.hpp"
#include "src/core/config.hpp"
#include "src/game/Entity.hpp"
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

struct PlayerInputState;

class Player : public Entity,
               public IDamageAble,
               public IDrawAble,
               public IMoveAble {
private:
  float m_shootTimer = 0.f;
  float m_shootDelay;
  sf::Vector2<float> m_velocity;
  float m_speed;
  sf::Vector2<float> m_mousePos;
  std::vector<std::unique_ptr<Weapon>> m_weapons;
  int m_activeWeapon = -1;
  mutable sf::Sprite m_weaponSprite;
  AnimatedSprite m_animator;

  void updateWeaponSprite(ResourceManager &rm);

public:
  Player(const sf::Texture &InitTexture, sf::Vector2<float> InitPos,
         float InitSpeed = config::PLAYER_DEFAULT_SPEED,
         float InitShootDelay = config::PLAYER_DEFAULT_SHOOT_DELAY);

  void initAnimation(int frameW, int frameH, int frameCount, float frameTime,
                     int idleRow = 0, int runRow = 1) {
    m_animator.init(frameW, frameH, frameCount, frameTime, idleRow, runRow);
    sprite.setTextureRect({0, idleRow * frameH, frameW, frameH});
    sprite.setOrigin(frameW / 2.f, frameH / 2.f);
  }

  virtual void move(const sf::Time &dt,
                    const sf::Vector2<float> &offset) override;
  virtual void update(const sf::Time &dt) override;
  virtual void draw(sf::RenderWindow &window) const override;
  virtual sf::FloatRect getHitbox() const override;
  virtual float getLayerY() const override;

  void moveShootTime(const sf::Time &dt);
  bool isShootTime() const;
  void cooldown();
  void setMousePos(const sf::Vector2<float> &worldMousePos);
  void setPosition(sf::Vector2<float> offset) { sprite.setPosition(offset); }
  void syncWeaponTransform();
  void handlePlayer(const PlayerInputState &input, ResourceManager &rm,
                    std::vector<std::unique_ptr<Bullet>> &bullets);

  virtual void takeDamage(float amount) override {}
  virtual float getHealth() override { return 0.f; }

  virtual float getSpeed() override { return m_speed; }
  virtual sf::Vector2<float> getVelocity() const override { return m_velocity; }
  sf::Vector2<float> getCenter() const {
    auto hb = getHitbox();
    return {hb.left + hb.width / 2.f, hb.top + hb.height / 2.f};
  }

  void addWeapon(std::unique_ptr<Weapon> w, ResourceManager &rm);
  void switchWeapon(int slot, ResourceManager &rm);
  int activeWeaponIndex() const { return m_activeWeapon; }
  size_t getWeaponCount() const { return m_weapons.size(); }
  std::unique_ptr<Weapon> removeWeapon(int slot);
  const std::optional<std::reference_wrapper<Weapon>> getActiveWeapon() const;
};

#endif
