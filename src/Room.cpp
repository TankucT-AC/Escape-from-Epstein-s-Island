// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Room.hpp"
#include "config.hpp"

Room::Room(const std::vector<std::vector<int>>& InitBlueprint, 
        sf::Vector2<float> InitPos, ResourceManager& rm)
{
    for (int y = 0; y < static_cast<int>(InitBlueprint.size()); ++y)
    {
        for (int x = 0; x < static_cast<int>(InitBlueprint[y].size()); ++x)
        {
            if (InitBlueprint[y][x] == static_cast<int>(RoomElements::WALL))
            {
                sf::Vector2<float> wall_pos = {
                    InitPos.x + (x * config::TILE_SIZE), 
                    InitPos.y + (y * config::TILE_SIZE)
                };
                walls.push_back(std::make_unique<Wall>(
                    rm.getTexture(config::DEFAULT_WALL_TEXTURE), 
                        wall_pos));
            }
        }
    }
}

void Room::draw(sf::RenderWindow& window)
{
    for (const auto& wall : walls)
    {
        wall->draw(window);
    }
}