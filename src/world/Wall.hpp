// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WALL_HPP
#define WALL_HPP

#include "src/game/Entity.hpp"

class Wall : public Entity
{
public:
    Wall(const sf::Texture& InitTexture, sf::Vector2<float> InitPos);

    virtual void move(const sf::Time& dt, const sf::Vector2<float>& offset) override {};
    virtual void update(const UpdateContext& ctx) override {};
    virtual void draw(sf::RenderWindow& window) override;
};

#endif // WALL_HPP