// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UPDATE_CONFIG_HPP
#define UPDATE_CONFIG_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>


class Room;

struct UpdateContext {
    const sf::Time& dt;       
    sf::RenderWindow& window; 
    Room& room;               

    UpdateContext(const sf::Time& d, sf::RenderWindow& w, Room& r) 
        : dt(d), window(w), room(r) {}
};

#endif