// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef GATE_HPP
#define GATE_HPP

#include "src/game/Entity.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

class Gate : public Entity, public IDrawAble {
public:
  enum class State { Open, Closed };

private:
  State m_state;
  sf::Texture m_openTex;
  sf::Texture m_closedTex;

public:
  Gate(const sf::Texture &openTex, const sf::Texture &closedTex,
       sf::Vector2<float> pos);

  void setOpen();
  void setClosed();
  bool isOpen() const { return m_state == State::Open; }

  virtual void update(const sf::Time &dt) override {}
  virtual void draw(sf::RenderWindow &window) const override;
  virtual float getLayerY() const override;
};

#endif
