// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "InputManager.hpp"
#include "src/core/ResourceManager.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

void InputManager::pollEvents(sf::RenderWindow &window, sf::Event &event,
                              sf::View &camera) {
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      window.close();

    if (event.type == sf::Event::Resized) {
      auto newWidth = static_cast<float>(event.size.width);
      auto newHeight = static_cast<float>(event.size.height);

      camera.setSize(newWidth, newHeight);
    }
  }
}

PlayerInputState InputManager::getPlayerInput(sf::RenderWindow &window,
                                              Player &player,
                                              ResourceManager &rm) {
  PlayerInputState state;
  sf::Vector2<int> pixelPos = sf::Mouse::getPosition(window);
  state.mousePos = window.mapPixelToCoords(pixelPos);

  if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    state.wantToShoot = true;

  if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    state.wantToInteract = true;

  return state;
}
