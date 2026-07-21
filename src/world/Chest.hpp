// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CHEST_HPP
#define CHEST_HPP

#include "src/game/Entity.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

class Chest : public Entity, public IDrawAble {
public:
  enum class State { Closed, Open };

private:
  State m_state;
  sf::Texture m_closedTex;
  sf::Texture m_openTex;

public:
  Chest(const sf::Texture &closedTex, const sf::Texture &openTex,
        sf::Vector2<float> pos);

  void open();
  bool isOpen() const { return m_state == State::Open; }

  virtual void update(const sf::Time &dt) override {}
  virtual void draw(sf::RenderWindow &window) const override;
  virtual float getLayerY() const override;
};

#endif
