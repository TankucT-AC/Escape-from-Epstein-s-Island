// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PORTAL_HPP
#define PORTAL_HPP

#include "src/game/AnimatedSprite.hpp"
#include "src/game/Entity.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

class Portal : public Entity, public IDrawAble {
private:
  bool m_active;
  AnimatedSprite m_animator;

public:
  Portal(const sf::Texture &tex, sf::Vector2<float> pos);

  void activate();
  bool isActive() const { return m_active; }

  virtual void update(const sf::Time &dt) override;
  virtual void draw(sf::RenderWindow &window) const override;
  virtual float getLayerY() const override;
};

#endif
