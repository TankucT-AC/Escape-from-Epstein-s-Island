// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include "src/core/UpdateContext.hpp"

// Абстракный класс для сущностей
class Entity 
{
protected:
    sf::Sprite sprite;
    float speed;
public:
    Entity(const sf::Texture& InitTexture, 
        float InitSpeed,
        sf::Vector2<float> InitPos) : 
    sprite(InitTexture), speed(InitSpeed) 
    { 
        sprite.setPosition(InitPos);
        
        auto bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    }
    virtual ~Entity() = default;

    virtual void move(const sf::Time& dt, const sf::Vector2<float>& offset) = 0;
    virtual void update(const UpdateContext& ctx) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    virtual sf::Vector2<float> getPosition() const { return sprite.getPosition(); };
    virtual sf::Rect<float> getHitbox() const { return sprite.getGlobalBounds(); };
    virtual void setPosition(const sf::Vector2<float>& newPos) { sprite.setPosition(newPos); };
};

#endif