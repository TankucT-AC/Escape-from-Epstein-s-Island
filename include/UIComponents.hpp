// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UI_COMPONENTS_HPP
#define UI_COMPONENTS_HPP

#include <SFML/Graphics.hpp>
#include <string>

namespace ui {
    struct Button 
    {
        sf::RectangleShape shape;
        sf::Text text;

        Button(const std::string& label, sf::Vector2f pos, sf::Font& font) 
        {
            shape.setSize({130.f, 40.f});
            shape.setPosition(pos);
            shape.setFillColor(sf::Color(60, 60, 60));

            text.setFont(font);
            text.setString(label);
            text.setCharacterSize(18);
            // Центрируем текст на кнопке
            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
            text.setPosition(pos.x + 65.f, pos.y + 20.f);
        }

        bool isHovered(sf::Vector2i mousePos) const 
        {
            return shape.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        }

        void update(sf::Vector2i mousePos) 
        {
            shape.setFillColor(isHovered(mousePos) ? sf::Color(100, 100, 100) : sf::Color(60, 60, 60));
        }

        void draw(sf::RenderWindow& window) 
        {
            window.draw(shape);
            window.draw(text);
        }
    };

    struct Layout {
        static constexpr float LeftMargin = 60.f;
        static constexpr float LabelWidth = 110.f; // Расстояние от начала текста до поля ввода
        static constexpr float TopOffset  = 60.f;
        static constexpr float VerticalSpacing = 50.f; // Дистанция между строками
        static constexpr float ButtonAreaY = 200.f;
        static constexpr float StatusAreaY = 320.f;
    };
}

#endif