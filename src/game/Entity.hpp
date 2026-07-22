// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

class IDamageAble {
public:
  virtual void takeDamage(float amount) = 0;
  virtual float getHealth() = 0;
};

class IDamageDealer {
public:
  virtual float getDamage() = 0;
};

class IMoveAble {
public:
  virtual void move(const sf::Time &dt, const sf::Vector2<float> &offset) = 0;
  virtual float getSpeed() = 0;
  virtual sf::Vector2<float> getVelocity() const = 0;
};

class IDrawAble {
public:
  virtual ~IDrawAble() = default;
  virtual void draw(sf::RenderWindow &window) const = 0;
  virtual float getLayerY() const = 0;
};

class Entity {
protected:
  sf::Sprite sprite;

public:
  Entity(const sf::Texture &InitTexture, sf::Vector2<float> InitPos)
      : sprite(InitTexture) {
    sprite.setPosition(InitPos);
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
  }
  virtual ~Entity() = default;

  virtual void update(const sf::Time &dt) = 0;

  virtual sf::Vector2<float> getPosition() const {
    return sprite.getPosition();
  }
  virtual sf::FloatRect getHitbox() const { return sprite.getGlobalBounds(); }
  virtual void setPosition(const sf::Vector2<float> &newPos) {
    sprite.setPosition(newPos);
  }

  sf::Sprite &getSprite() { return sprite; }
  const sf::Sprite &getSprite() const { return sprite; }
};

#endif // ENTITY_HPP