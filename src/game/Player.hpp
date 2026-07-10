// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Bullet.hpp"
#include "src/core/ResourceManager.hpp"
#include "src/core/config.hpp"
#include "src/game/Entity.hpp"
#include <memory>
#include <vector>

// Forward-ссылка на PlayerInputState
struct PlayerInputState;

class Player : public Entity,
               public IDamageAble,
               public IDrawAble,
               public IMoveAble {
private:
  float ShootTime;
  float ShootDelay;
  sf::Vector2<float> velocity;
  float speed;
  sf::Vector2<float> mousePos;

public:
  Player(const sf::Texture &InitTexture, sf::Vector2<float> InitPos,
         float InitSpeed = config::PLAYER_DEFAULT_SPEED,
         float InitShootDelay = config::PLAYER_DEFAULT_SHOOT_DELAY);

  virtual void move(const sf::Time &dt,
                    const sf::Vector2<float> &offset) override;
  virtual void update(const sf::Time &dt) override;
  virtual void draw(sf::RenderWindow &window) const override;
  virtual sf::Rect<float> getHitbox() const override;
  virtual float getLayerY() const override;

  virtual void moveShootTime(const sf::Time &dt);
  virtual bool isShootTime() const;
  virtual void cooldown();
  virtual void setMousePos(const sf::Vector2<float> &worldMousePos);
  void setPosition(sf::Vector2<float> offset) { sprite.setPosition(offset); }
  void handlePlayer(const PlayerInputState &input, ResourceManager &rm,
                    std::vector<std::unique_ptr<Bullet>> &bullets);

  virtual void takeDamage(float amount) override {}
  virtual float getHealth() override { return 0.f; }

  virtual float getSpeed() override { return speed; }
  virtual sf::Vector2<float> getVelocity() const override { return velocity; }
};

#endif
