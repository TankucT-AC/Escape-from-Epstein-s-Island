// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Room.hpp"
#include "src/core/RenderManager.hpp"
#include "src/core/config.hpp"
#include "src/world/Prefabs.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <string>
#include <vector>

namespace {
const int ROOM_SZ = Prefabs::PREFAB_SIZE;
const int DOOR_IDX = Prefabs::DOOR_INDEX;

// Превращает двумерный префаб в плоскую строку
std::string flattenPrefab(uint8_t index) {
  const auto &prefab = Prefabs::ALL[index];
  std::string flat;
  for (const auto &row : prefab)
    flat += row;
  return flat;
}

// Создаёт Wall по тайловым координатам
void addWall(std::vector<std::unique_ptr<Wall>> &walls, int tx, int ty,
             const sf::Texture &tex, bool sortable) {
  sf::Vector2<float> pos = {tx * config::TILE_SIZE + config::TILE_SIZE / 2.f,
                            ty * config::TILE_SIZE + config::TILE_SIZE / 2.f};
  walls.push_back(std::make_unique<Wall>(tex, pos, sortable));
}

// Создаёт Floor по тайловым координатам
void addFloor(std::vector<std::unique_ptr<Floor>> &floors, int tx, int ty,
              const sf::Texture &tex) {
  sf::Vector2<float> pos = {tx * config::TILE_SIZE + config::TILE_SIZE / 2.f,
                            ty * config::TILE_SIZE + config::TILE_SIZE / 2.f};
  floors.push_back(std::make_unique<Floor>(tex, pos));
}
} // namespace

// ─── Старый конструктор (от сетки int-ов) ───
Room::Room(const std::vector<std::vector<int>> &InitBlueprint,
           sf::Vector2<float> InitPos, ResourceManager &rm) {
  for (int y = 0; y < static_cast<int>(InitBlueprint.size()); ++y) {
    for (int x = 0; x < static_cast<int>(InitBlueprint[y].size()); ++x) {
      int tile = InitBlueprint[y][x];
      if (tile == static_cast<int>(RoomElements::TEMP_FLAG))
        continue;
      if (tile == static_cast<int>(RoomElements::WALL)) {
        sf::Vector2<float> wall_pos = {InitPos.x + (x * config::TILE_SIZE),
                                       InitPos.y + (y * config::TILE_SIZE)};
        bool isSortable =
            ((y < 0 && InitBlueprint[y - 1][x] ==
                           static_cast<int>(RoomElements::FLOOR)));
        walls.push_back(std::make_unique<Wall>(
            rm.getTexture(config::DEFAULT_WALL_TEXTURE), wall_pos, isSortable));
      } else if (InitBlueprint[y][x] == static_cast<int>(RoomElements::FLOOR)) {
        sf::Vector2<float> floor_pos = {InitPos.x + (x * config::TILE_SIZE),
                                        InitPos.y + (y * config::TILE_SIZE)};
        floors.push_back(std::make_unique<Floor>(
            rm.getTexture(config::DESERT_FLOOR_TEXTURE), floor_pos));
      }
    }
  }
}

Room::Room(uint8_t prefabIndex, sf::Vector2<int> tilePos, bool cUp, bool cDown,
           bool cLeft, bool cRight, int doorHalfW, ResourceManager &rm) {
  const std::string prefab = flattenPrefab(prefabIndex);
  const sf::Texture &wallTex = rm.getTexture(config::DEFAULT_WALL_TEXTURE);
  const sf::Texture &floorTex = rm.getTexture(config::DESERT_FLOOR_TEXTURE);

  // Сначала вычисляем, какие тайлы — пол, а какие — стена.
  // Используем временную bool-сетку: true = пол, false = стена.
  std::vector<std::vector<bool>> isFloor(ROOM_SZ,
                                         std::vector<bool>(ROOM_SZ, false));

  for (int py = 0; py < ROOM_SZ; ++py) {
    for (int px = 0; px < ROOM_SZ; ++px) {
      char ch = prefab[py * ROOM_SZ + px];

      // Дверные позиции: центр каждой из 4 стен
      bool isTopDoor = (py == 0 && px == DOOR_IDX);
      bool isBottomDoor = (py == ROOM_SZ - 1 && px == DOOR_IDX);
      bool isLeftDoor = (py == DOOR_IDX && px == 0);
      bool isRightDoor = (py == DOOR_IDX && px == ROOM_SZ - 1);

      bool connected = false;
      if (isTopDoor)
        connected = cUp;
      else if (isBottomDoor)
        connected = cDown;
      else if (isLeftDoor)
        connected = cLeft;
      else if (isRightDoor)
        connected = cRight;

      bool atDoor = isTopDoor || isBottomDoor || isLeftDoor || isRightDoor;

      if (atDoor && connected) {
        // Соединённая дверь — пол + уширение на doorHalfW в обе стороны
        if (isTopDoor || isBottomDoor) {
          for (int ox = -doorHalfW; ox <= doorHalfW; ++ox) {
            int gx = px + ox;
            if (gx >= 0 && gx < ROOM_SZ)
              isFloor[py][gx] = true;
          }
        } else {
          for (int oy = -doorHalfW; oy <= doorHalfW; ++oy) {
            int gy = py + oy;
            if (gy >= 0 && gy < ROOM_SZ)
              isFloor[gy][px] = true;
          }
        }
        continue;
      }

      if (atDoor && !connected) {
        // Запечатанная дверь — стена (isFloor уже false)
        continue;
      }

      // Обычный тайл: не перезаписываем, если уже пол (уширение двери)
      if (isFloor[py][px])
        continue;

      if (ch == '#' || ch == 'O')
        isFloor[py][px] = false;
      else
        isFloor[py][px] = true;
    }
  }

  // Строим Wall/Floor по готовой bool-сетке
  for (int py = 0; py < ROOM_SZ; ++py) {
    for (int px = 0; px < ROOM_SZ; ++px) {
      int tx = tilePos.x + px;
      int ty = tilePos.y + py;

      if (isFloor[py][px]) {
        addFloor(floors, tx, ty, floorTex);
      } else {
        bool sortable = (py > 0 && isFloor[py - 1][px]);
        addWall(walls, tx, ty, wallTex, sortable);
      }
    }
  }
}

// ─── Отрисовка ───
void Room::submitRender(RenderManager &RenderManager) {
  for (const auto &wall : walls) {
    RenderManager.submit(*wall);
  }

  for (const auto &floor : floors) {
    RenderManager.submit(*floor);
  }
}

bool Room::isObjectInRoom(sf::Vector2<float> position) {
  if (floors.empty())
    return false;

  float max_left = walls.front()->getHitbox().left;
  float max_top = walls.front()->getHitbox().top;
  float wall_width = walls.front()->getHitbox().width;
  float wall_height = walls.front()->getHitbox().height;

  for (const auto &wall : walls) {
    sf::Rect<float> hitbox = wall->getHitbox();
    if (hitbox.left > max_left)
      max_left = hitbox.left;
    if (hitbox.top > max_top)
      max_top = hitbox.top;
  }

  sf::Vector2<float> basePoint = {max_left, max_top};

  float min_x = basePoint.x - (ROOM_SZ - 2) * wall_width;
  float min_y = basePoint.y - (ROOM_SZ - 2) * wall_height;

  bool result = (position.x >= min_x && position.x <= basePoint.x) &&
                (position.y >= min_y && position.y <= basePoint.y);
  return result;
}
