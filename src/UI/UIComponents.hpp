// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UI_COMPONENTS_HPP
#define UI_COMPONENTS_HPP

#include <SFML/Graphics.hpp>
#include <string>

namespace ui {

// Стили Discord
const sf::Color ColorBlurple(88, 101, 242);
const sf::Color ColorBlurpleHover(114, 137, 218);
const sf::Color ColorDarkBG(43, 45, 49);
const sf::Color ColorPanelBG(49, 51, 56);
const sf::Color ColorInputBG(30, 31, 34);
const sf::Color ColorTextGray(185, 187, 190);
const sf::Color ColorError(237, 66, 69);
const sf::Color ColorSuccess(87, 242, 135);

struct Button {
  sf::RectangleShape box;
  sf::Text text;
  sf::Color baseColor;
  sf::Color hoverColor;
  bool isVisible = true;

  void init(const std::string &label, sf::Vector2f pos, sf::Vector2f size,
            sf::Font &font, sf::Color base = ColorBlurple,
            sf::Color hover = ColorBlurpleHover) {
    box.setSize(size);
    box.setPosition(pos);
    baseColor = base;
    hoverColor = hover;
    box.setFillColor(base);

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);

    centerText();
  }

  void centerText() {
    sf::FloatRect r = text.getLocalBounds();
    text.setOrigin(r.left + r.width / 2.f, r.top + r.height / 2.f);
    text.setPosition(box.getPosition().x + box.getSize().x / 2.f,
                     box.getPosition().y + box.getSize().y / 2.f - 2.f);
  }

  void setLabel(const std::string &newLabel) {
    text.setString(newLabel);
    centerText();
  }

  bool contains(sf::Vector2i m) {
    return isVisible && box.getGlobalBounds().contains(m.x, m.y);
  }
  void update(sf::Vector2i m) {
    if (isVisible)
      box.setFillColor(contains(m) ? hoverColor : baseColor);
  }
  void draw(sf::RenderWindow &w) {
    if (isVisible) {
      w.draw(box);
      w.draw(text);
    }
  }
};

struct Input {
  sf::RectangleShape box;
  sf::Text label;
  sf::Text display;
  std::string value;
  bool isPassword = false, isFocused = false, isVisible = true;

  void init(const std::string &labelStr, sf::Vector2f pos, sf::Font &font,
            bool pass = false) {
    box.setSize({340, 45});
    box.setPosition(pos);
    box.setFillColor(ColorInputBG);
    box.setOutlineThickness(2);
    box.setOutlineColor(sf::Color(30, 31, 34));

    label.setFont(font);
    label.setString(labelStr);
    label.setCharacterSize(12);
    label.setFillColor(ColorTextGray);
    label.setPosition(pos.x, pos.y - 22);

    display.setFont(font);
    display.setCharacterSize(18);
    display.setFillColor(sf::Color::White);
    display.setPosition(pos.x + 12, pos.y + 8);
    isPassword = pass;
  }

  void handleText(sf::Uint32 u) {
    if (!isFocused || !isVisible)
      return;
    if (u == 8) {
      if (!value.empty())
        value.pop_back();
    } else if (u >= 32 && u < 127)
      value += static_cast<char>(u);
    updateVisuals();
  }

  void updateVisuals() {
    box.setOutlineColor(isFocused ? ColorBlurple : sf::Color(30, 31, 34));
    std::string d = isPassword ? std::string(value.length(), '*') : value;
    display.setString(d + (isFocused ? "_" : ""));
  }

  bool contains(sf::Vector2i m) {
    return isVisible && box.getGlobalBounds().contains(m.x, m.y);
  }
  void draw(sf::RenderWindow &w) {
    if (isVisible) {
      w.draw(label);
      w.draw(box);
      w.draw(display);
    }
  }
};
} // namespace ui
#endif