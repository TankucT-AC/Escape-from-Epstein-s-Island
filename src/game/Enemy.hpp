// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Bullet.hpp"
#include "Entity.hpp"
#include "src/core/config.hpp"
#include <SFML/System/Vector2.hpp>

class Enemy : public Entity,
              public IDamageAble,
              public IDrawAble,
              public IMoveAble {
private:
  float shootTime;
  float shootDelay;
  float health;
  bool dead;
  float speed;
  sf::Vector2<float> velocity;

public:
  Enemy(const sf::Texture &InitTexture, sf::Vector2<float> InitPos,
        float InitSpeed = config::ENEMY_DEFAULT_SPEED,
        float InitShootDelay = config::PLAYER_DEFAULT_SHOOT_DELAY,
        float InitHealth = config::ENEMY_DEFAULT_HEALTH);

  virtual void move(const sf::Time &dt,
                    const sf::Vector2<float> &offset) override;
  virtual void update(const UpdateContext &ctx) override {};
  virtual void draw(sf::RenderWindow &window) override;

  virtual void update(const sf::Time &dt, const sf::Vector2<float> &playerPos,
                      sf::RenderWindow &window);
  virtual void takeDamage(float amount) override;

  virtual float getHealth() override { return health; }
  bool isEnemyAlive() const { return !dead; }
  bool isBulletCollision(const Bullet &bullet);

  virtual float getSpeed() override { return speed; }
  virtual sf::Vector2<float> getVelocity() override { return velocity; }
};

#endif
