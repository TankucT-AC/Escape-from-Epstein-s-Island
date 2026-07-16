#ifndef FLOOR_HPP
#define FLOOR_HPP

#include "src/game/Entity.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

class Floor : public Entity, public IDrawAble {
public:
  Floor(const sf::Texture &InitTexture, sf::Vector2<float> InitPos);
  virtual void update(const sf::Time &dt) override {}
  virtual void draw(sf::RenderWindow &window) const override;
  virtual float getLayerY() const override;
};

#endif // FLOOR_HPP
