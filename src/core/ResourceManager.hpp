// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <stdexcept>
#include <string>

class ResourceManager {
private:
    std::map<std::string, sf::Texture> textures;

public:
    sf::Texture& getTexture(const std::string& path) 
    {
        auto it = textures.find(path);
        if (it != textures.end()) 
        {
            return it->second;
        }

        sf::Texture tex;
        if (!tex.loadFromFile(path)) 
        {
            throw std::runtime_error("Image is not found");
        }
        
        textures[path] = std::move(tex);
        return textures[path];
    }
};

#endif