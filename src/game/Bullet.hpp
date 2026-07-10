// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BULLET_HPP
#define BULLET_HPP

#include "Entity.hpp"
#include "src/core/config.hpp"

class Bullet : public Entity,
               public IDamageDealer,
               public IDrawAble,
               public IMoveAble {
private:
  float damage;
  float lifetime;
  sf::Vector2<float> direction;
  bool dead;
  float speed;
  sf::Vector2<float> velocity;

public:
  Bullet(const sf::Texture &InitTexture, const sf::Vector2<float> &InitStartPos,
         const sf::Vector2<float> &InitDirection, float InitDegree = 0.f,
         float InitSpeed = config::BULLET_DEFAULT_SPEED,
         float InitLifetime = config::BULLET_DEFAULT_LIFETIME,
         float InitDamage = config::BULLET_DEFAULT_DAMAGE);

  virtual void update(const sf::Time &dt) override;
  virtual void draw(sf::RenderWindow &window) const override;
  virtual void move(const sf::Time &dt,
                    const sf::Vector2<float> &offset) override;
  virtual sf::Rect<float> getHitbox() const override;
  virtual float getLayerY() const override;

  int getLifetime() const;
  bool isBulletAlive() const;
  virtual float getDamage() override;

  virtual float getSpeed() override { return speed; }
  virtual sf::Vector2<float> getVelocity() const override { return velocity; }
  void setBulletAlive(bool isAlive) { dead = !isAlive; };
};

#endif
