// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Room.hpp"
#include "src/core/config.hpp"
#include "src/world/Prefabs.hpp"
#include <memory>
#include <string>
#include <vector>

namespace {
const int ROOM_SZ = Prefabs::PREFAB_SIZE;
const int DOOR_IDX = Prefabs::DOOR_INDEX;

std::string flattenPrefab(uint8_t index) {
  const auto &prefab = Prefabs::ALL[index];
  std::string flat;
  for (const auto &row : prefab)
    flat += row;
  return flat;
}

sf::Vector2<float> tileToWorld(int tx, int ty) {
  return {tx * config::TILE_SIZE + config::TILE_SIZE / 2.f,
          ty * config::TILE_SIZE + config::TILE_SIZE / 2.f};
}
} // namespace

// ─── Старый конструктор ───
Room::Room(const std::vector<std::vector<int>> &InitBlueprint,
           sf::Vector2<float> InitPos, ResourceManager &rm)
    : m_tilePos{0, 0}, m_roomType(RoomType::Spawn),
      m_combatState(CombatState::Inactive) {
  for (int y = 0; y < static_cast<int>(InitBlueprint.size()); ++y) {
    for (int x = 0; x < static_cast<int>(InitBlueprint[y].size()); ++x) {
      int tile = InitBlueprint[y][x];
      if (tile == static_cast<int>(RoomElements::TEMP_FLAG))
        continue;
      if (tile == static_cast<int>(RoomElements::WALL)) {
        sf::Vector2<float> wp = {InitPos.x + (x * config::TILE_SIZE),
                                 InitPos.y + (y * config::TILE_SIZE)};
        m_walls.push_back(std::make_unique<Wall>(
            rm.getTexture(config::DEFAULT_WALL_TEXTURE), wp, false));
      } else if (tile == static_cast<int>(RoomElements::FLOOR)) {
        sf::Vector2<float> fp = {InitPos.x + (x * config::TILE_SIZE),
                                 InitPos.y + (y * config::TILE_SIZE)};
        m_floors.push_back(std::make_unique<Floor>(
            rm.getTexture(config::DESERT_FLOOR_TEXTURE), fp));
      }
    }
  }
}

// ─── Основной конструктор ───
Room::Room(uint8_t prefabIndex, sf::Vector2<int> tilePos, bool cUp, bool cDown,
           bool cLeft, bool cRight, int doorHalfW, ResourceManager &rm)
    : m_tilePos(tilePos), m_roomType(typeFromPrefab(prefabIndex)),
      m_combatState(CombatState::Inactive) {
  buildFromPrefab(prefabIndex, cUp, cDown, cLeft, cRight, doorHalfW, rm);
}

void Room::buildFromPrefab(uint8_t prefabIndex, bool cUp, bool cDown,
                            bool cLeft, bool cRight, int doorHalfW,
                            ResourceManager &rm) {
  const std::string prefab = flattenPrefab(prefabIndex);
  const sf::Texture &wallTex = rm.getTexture(config::DEFAULT_WALL_TEXTURE);
  const sf::Texture &floorTex = rm.getTexture(config::DESERT_FLOOR_TEXTURE);

  std::vector<std::vector<bool>> isFloor(ROOM_SZ,
                                         std::vector<bool>(ROOM_SZ, false));

  // Проход 1: разметка пол/стена, спавн-точки, ворота
  for (int py = 0; py < ROOM_SZ; ++py) {
    for (int px = 0; px < ROOM_SZ; ++px) {
      char ch = prefab[py * ROOM_SZ + px];

      bool isTop = (py == 0 && px == DOOR_IDX);
      bool isBot = (py == ROOM_SZ - 1 && px == DOOR_IDX);
      bool isLef = (py == DOOR_IDX && px == 0);
      bool isRig = (py == DOOR_IDX && px == ROOM_SZ - 1);

      bool connected = false;
      if (isTop)
        connected = cUp;
      else if (isBot)
        connected = cDown;
      else if (isLef)
        connected = cLeft;
      else if (isRig)
        connected = cRight;

      bool atDoor = isTop || isBot || isLef || isRig;

      if (atDoor && connected) {
        if (isTop || isBot) {
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

      if (atDoor && !connected)
        continue;

      if (isFloor[py][px])
        continue;

      if (ch == 'x')
        m_spawnPoints.push_back(
            tileToWorld(m_tilePos.x + px, m_tilePos.y + py));

      if (ch == '#' || ch == 'O')
        isFloor[py][px] = false;
      else
        isFloor[py][px] = true;
    }
  }

  // Сундук: ищем пустой пол в центре
  if (m_roomType == RoomType::Combat || m_roomType == RoomType::Chest) {
    if (isFloor[DOOR_IDX][DOOR_IDX]) {
      m_chestPos = tileToWorld(m_tilePos.x + DOOR_IDX, m_tilePos.y + DOOR_IDX);
      m_hasChestSlot = true;
    } else {
      for (int py = DOOR_IDX - 1; py <= DOOR_IDX + 1 && !m_hasChestSlot; ++py)
        for (int px = DOOR_IDX - 1; px <= DOOR_IDX + 1; ++px)
          if (py >= 0 && py < ROOM_SZ && px >= 0 && px < ROOM_SZ &&
              isFloor[py][px]) {
            m_chestPos = tileToWorld(m_tilePos.x + px, m_tilePos.y + py);
            m_hasChestSlot = true;
          }
    }
  }

  // Создаём ворота на соединённых дверях
  const sf::Texture &gateOpenTex = rm.getTexture(config::DEFAUTL_GATE_OPEN);
  const sf::Texture &gateClosedTex = rm.getTexture(config::DEFAULT_GATE_CLOSED);
  int gateW = doorHalfW * 2 + 1;

  auto addGates = [&](int gx, int gy, bool horiz) {
    sf::Vector2<float> center =
        tileToWorld(m_tilePos.x + gx, m_tilePos.y + gy);
    int half = gateW / 2;
    float step = config::TILE_SIZE;
    if (horiz) {
      for (int ox = -half; ox <= half; ++ox)
        m_gates.push_back(std::make_unique<Gate>(
            gateOpenTex, gateClosedTex,
            sf::Vector2<float>{center.x + ox * step, center.y}));
    } else {
      for (int oy = -half; oy <= half; ++oy)
        m_gates.push_back(std::make_unique<Gate>(
            gateOpenTex, gateClosedTex,
            sf::Vector2<float>{center.x, center.y + oy * step}));
    }
  };

  if (cUp)
    addGates(DOOR_IDX, 0, true);
  if (cDown)
    addGates(DOOR_IDX, ROOM_SZ - 1, true);
  if (cLeft)
    addGates(0, DOOR_IDX, false);
  if (cRight)
    addGates(ROOM_SZ - 1, DOOR_IDX, false);

  // Проход 3: строим Wall/Floor
  for (int py = 0; py < ROOM_SZ; ++py) {
    for (int px = 0; px < ROOM_SZ; ++px) {
      int tx = m_tilePos.x + px;
      int ty = m_tilePos.y + py;
      if (isFloor[py][px]) {
        m_floors.push_back(std::make_unique<Floor>(
            floorTex,
            sf::Vector2<float>{tx * config::TILE_SIZE + config::TILE_SIZE / 2.f,
                               ty * config::TILE_SIZE + config::TILE_SIZE / 2.f}));
      } else {
        bool sortable = (py > 0 && isFloor[py - 1][px]);
        m_walls.push_back(std::make_unique<Wall>(
            wallTex,
            sf::Vector2<float>{tx * config::TILE_SIZE + config::TILE_SIZE / 2.f,
                               ty * config::TILE_SIZE + config::TILE_SIZE / 2.f},
            sortable));
      }
    }
  }

  // Сундук для Chest-комнат создаём сразу
  if (m_roomType == RoomType::Chest && m_hasChestSlot)
    spawnChest(rm);
}

void Room::spawnChest(ResourceManager &rm) {
  if (!m_hasChestSlot || m_chest)
    return;
  m_chest = std::make_unique<Chest>(
      rm.getTexture(config::DEFAUTL_CHEST_CLOSED_TEXTURE),
      rm.getTexture(config::DEFAUTL_CHEST_OPEN_TEXTURE), m_chestPos);
}

void Room::submitRender(RenderManager &rm) {
  for (const auto &w : m_walls)
    rm.submit(*w);
  for (const auto &f : m_floors)
    rm.submit(*f);
  for (const auto &g : m_gates)
    rm.submit(*g);
  if (m_chest)
    rm.submit(*m_chest);
}

bool Room::isObjectInRoom(sf::Vector2<float> position) {
  if (m_floors.empty())
    return false;

  float max_left = m_walls.front()->getHitbox().left;
  float max_top = m_walls.front()->getHitbox().top;
  float wall_width = m_walls.front()->getHitbox().width;
  float wall_height = m_walls.front()->getHitbox().height;

  for (const auto &w : m_walls) {
    auto hb = w->getHitbox();
    if (hb.left > max_left)
      max_left = hb.left;
    if (hb.top > max_top)
      max_top = hb.top;
  }

  float min_x = max_left - (ROOM_SZ - 2) * wall_width;
  float min_y = max_top - (ROOM_SZ - 2) * wall_height;

  return (position.x >= min_x && position.x <= max_left) &&
         (position.y >= min_y && position.y <= max_top);
}

RoomType Room::typeFromPrefab(uint8_t prefabIndex) {
  if (prefabIndex == 1)
    return RoomType::Spawn;
  if (prefabIndex == 2 || prefabIndex == 6)
    return RoomType::Combat;
  return RoomType::Chest;
}