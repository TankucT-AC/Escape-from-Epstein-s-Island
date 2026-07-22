// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "AnimatedSprite.hpp"
#include "Entity.hpp"
#include "src/core/config.hpp"
#include <SFML/System/Vector2.hpp>

class Bullet;

class Enemy : public Entity,
              public IDamageAble,
              public IDrawAble,
              public IMoveAble {
private:
  float health;
  bool dead;
  float speed;
  sf::Vector2<float> velocity;
  sf::Vector2<float> targetPosition;
  AnimatedSprite m_animator;

public:
  Enemy(const sf::Texture &InitTexture, sf::Vector2<float> InitPos,
        float InitSpeed = config::ENEMY_DEFAULT_SPEED,
        float InitHealth = config::ENEMY_DEFAULT_HEALTH);

  void initAnimation(int frameW, int frameH, int frameCount, float frameTime, int idleRow = 0, int runRow = 0) {
    m_animator.init(frameW, frameH, frameCount, frameTime, idleRow, runRow);
    sprite.setTextureRect({0, idleRow * frameH, frameW, frameH});
    sprite.setOrigin(frameW / 2.f, frameH / 2.f);
  }

  virtual void move(const sf::Time &dt,
                    const sf::Vector2<float> &offset) override;
  virtual void update(const sf::Time &dt) override;
  virtual void draw(sf::RenderWindow &window) const override;
  virtual void takeDamage(float amount) override;
  virtual float getHealth() override { return health; }
  virtual float getLayerY() const override;

  bool isEnemyAlive() const { return !dead; }
  bool isBulletCollision(const Bullet &bullet);
  void setTargetPosition(const sf::Vector2<float> &targetPos) {
    targetPosition = targetPos;
  }

  virtual float getSpeed() override { return speed; }
  virtual sf::Vector2<float> getVelocity() const override { return velocity; }
};

#endif