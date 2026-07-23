// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WALL_HPP
#define WALL_HPP

#include "src/game/Entity.hpp"

class Wall : public Entity, public IDrawAble {
private:
  bool m_isSortable;

public:
  Wall(const sf::Texture &InitTexture, sf::Vector2<float> InitPos,
       bool isSortable = true);

  virtual void update(const sf::Time &dt) override {};
  virtual void draw(sf::RenderWindow &window) const override;
  virtual float getLayerY() const override;
};

#endif // WALL_HPP
