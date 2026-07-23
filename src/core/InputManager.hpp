// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "ResourceManager.hpp"
#include "src/game/Player.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

struct PlayerInputState {
  bool wantToShoot = false;
  bool wantToInteract = false;
  int weaponSlot = -1; // -1 = бездействие, 0/1/2 = Num1/Num2/Num3
  sf::Vector2<float> mousePos;
};

class InputManager {
private:
  bool m_prevRightButton = false;

public:
  void pollEvents(sf::RenderWindow &window, sf::Event &event, sf::View &camera);
  PlayerInputState getPlayerInput(sf::RenderWindow &window, Player &player,
                                  ResourceManager &rm);
};

#endif
