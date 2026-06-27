#ifndef PHYSICS_MANAGER_HPP
#define PHYSICS_MANAGER_HPP

#include <memory>
#include <vector>

class Room;
class Enemy;
class Bullet;

class PhysicsManager {
public:
  void handleCollisions(std::vector<std::unique_ptr<Bullet>> &bullets,
                        std::vector<std::unique_ptr<Enemy>> &enemies,
                        Room &room);

  void cleanup(std::vector<std::unique_ptr<Bullet>> &bullets,
               std::vector<std::unique_ptr<Enemy>> &enemies, Room &room);
};

#endif // PHYSICS_MANAGER_HPP
