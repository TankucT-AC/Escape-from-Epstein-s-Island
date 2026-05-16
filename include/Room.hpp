#ifndef ROOM_HPP
#define ROOM_HPP

#include "ResourceManager.hpp"
#include "Wall.hpp"
#include <memory>

enum class RoomElements
{
    FLOOR,
    WALL,
    TEMP_FLAG
};

class Room
{
private:
    std::vector<std::unique_ptr<Wall>> walls;
    sf::Vector2<float> position;

public:
    Room(const std::vector<std::vector<int>>& InitBlueprint, 
        sf::Vector2<float> InitPos, ResourceManager& rm);

    void draw(sf::RenderWindow& window);

    template <typename Object>
    bool checkCollision(const Object& object) const
    { 
        auto objectHitbox = object.getHitbox();
        for (const auto& wall : walls)
        {
            auto wallHitbox = wall->getHitbox();
            if (wallHitbox.intersects(objectHitbox)) return true;
        }

        return false;
    }

    sf::Vector2<float> getPosition() const { return walls.front()->getPosition(); }
};

#endif // ROOM_HPP