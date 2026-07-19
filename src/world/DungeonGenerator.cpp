// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DungeonGenerator.hpp"
#include "Prefabs.hpp"
#include "src/core/config.hpp"
#include <algorithm>
#include <random>

namespace {
constexpr int TILE_FLOOR = 0;
constexpr int TILE_WALL = 1;
constexpr int TILE_VOID = 2;
} // namespace

DungeonGenerator::DungeonGenerator(const DungeonConfig &config)
    : m_config(config) {
  if (m_config.seed == 0)
    m_config.seed = std::random_device{}();
  initPrefabs();
}

DungeonConfig DungeonGenerator::defaultConfig() {
  DungeonConfig cfg;
  cfg.gridWidth = 5;
  cfg.gridHeight = 4;
  cfg.walkSteps = 6;
  cfg.corridorLength = 6;
  cfg.corridorWidth = 3;
  cfg.prefabSize = 13;
  cfg.seed = 0;
  return cfg;
}

// Превращает каждый префаб в плоскую строку.
void DungeonGenerator::initPrefabs() {
  m_prefabs.clear();
  for (const auto &prefab : Prefabs::ALL) {
    std::string flat;
    for (const auto &row : prefab)
      flat += row;
    m_prefabs.push_back(flat);
  }
}

DungeonData DungeonGenerator::generate() {
  m_cells.clear();
  m_cells.resize(m_config.gridWidth * m_config.gridHeight);
  for (int y = 0; y < m_config.gridHeight; ++y)
    for (int x = 0; x < m_config.gridWidth; ++x) {
      Cell &c = m_cells[cellIndex(x, y)];
      c.x = x;
      c.y = y;
      c.active = false;
      c.connectUp = false;
      c.connectDown = false;
      c.connectLeft = false;
      c.connectRight = false;
    }

  m_walkPath.clear();
  randomWalk();
  resolveConnections();

  DungeonData data;
  assembleData(data);
  return data;
}

int DungeonGenerator::cellIndex(int x, int y) const {
  return y * m_config.gridWidth + x;
}

// ──────────────────────────────────────────────────────────────────
//  randomWalk() — «Пьяная походка» по макро-сетке
// ──────────────────────────────────────────────────────────────────
void DungeonGenerator::randomWalk() {
  std::mt19937 rng(m_config.seed);
  std::uniform_int_distribution<int> xDist(0, m_config.gridWidth - 1);
  std::uniform_int_distribution<int> yDist(0, m_config.gridHeight - 1);
  std::uniform_int_distribution<int> dirDist(0, 3);
  std::uniform_int_distribution<uint8_t> prefabDist(
      2, static_cast<uint8_t>(Prefabs::ALL.size() - 1));

  const int dx[4] = {0, 0, -1, 1};
  const int dy[4] = {-1, 1, 0, 0};

  int cx = xDist(rng);
  int cy = yDist(rng);
  m_cells[cellIndex(cx, cy)].active = true;
  m_cells[cellIndex(cx, cy)].prefabIndex = 1;
  m_walkPath.push_back({cx, cy});

  for (int step = 1; step < m_config.walkSteps; ++step) {
    int dirs[4] = {0, 1, 2, 3};
    std::shuffle(std::begin(dirs), std::end(dirs), rng);

    bool moved = false;
    for (int d : dirs) {
      int nx = cx + dx[d];
      int ny = cy + dy[d];

      if (nx < 0 || nx >= m_config.gridWidth || ny < 0 ||
          ny >= m_config.gridHeight)
        continue;
      if (m_cells[cellIndex(nx, ny)].active)
        continue;

      cx = nx;
      cy = ny;
      m_cells[cellIndex(cx, cy)].active = true;
      m_cells[cellIndex(cx, cy)].prefabIndex = prefabDist(rng);
      m_walkPath.push_back({cx, cy});
      moved = true;
      break;
    }

    if (!moved)
      break;
  }
}

// ──────────────────────────────────────────────────────────────────
//  resolveConnections() — установка флагов соединений по маршруту
// ──────────────────────────────────────────────────────────────────
void DungeonGenerator::resolveConnections() {
  for (size_t i = 0; i + 1 < m_walkPath.size(); ++i) {
    int x1 = m_walkPath[i].first;
    int y1 = m_walkPath[i].second;
    int x2 = m_walkPath[i + 1].first;
    int y2 = m_walkPath[i + 1].second;

    if (x1 == x2) {
      if (y1 < y2) {
        m_cells[cellIndex(x1, y1)].connectDown = true;
        m_cells[cellIndex(x2, y2)].connectUp = true;
      } else {
        m_cells[cellIndex(x1, y1)].connectUp = true;
        m_cells[cellIndex(x2, y2)].connectDown = true;
      }
    } else if (y1 == y2) {
      if (x1 < x2) {
        m_cells[cellIndex(x1, y1)].connectRight = true;
        m_cells[cellIndex(x2, y2)].connectLeft = true;
      } else {
        m_cells[cellIndex(x1, y1)].connectLeft = true;
        m_cells[cellIndex(x2, y2)].connectRight = true;
      }
    }
  }
}

// ──────────────────────────────────────────────────────────────────
//  assembleData() — заполняет списки комнат и коридоров
// ──────────────────────────────────────────────────────────────────
void DungeonGenerator::assembleData(DungeonData &data) {
  const int corrLen = m_config.corridorLength;
  const int corrWid = m_config.corridorWidth;
  const int roomSz = m_config.prefabSize;
  const int doorIdx = Prefabs::DOOR_INDEX;
  const int halfW = corrWid / 2;
  const int gapSize = (corrLen > 0) ? corrLen + 2 : 0;
  const int stride = (corrLen > 0) ? roomSz + gapSize : roomSz - 1;

  data.rooms.clear();
  data.corridors.clear();
  data.corridorWidth = m_config.corridorWidth;
  bool playerSpawnFound = false;

  // ── ЭТАП 1: размещение комнат ──
  for (int cy = 0; cy < m_config.gridHeight; ++cy) {
    for (int cx = 0; cx < m_config.gridWidth; ++cx) {
      const Cell &cell = m_cells[cellIndex(cx, cy)];
      if (!cell.active)
        continue;

      RoomPlacement rp;
      rp.prefabIndex = cell.prefabIndex;
      rp.tileX = cx * stride;
      rp.tileY = cy * stride;
      rp.connectUp = cell.connectUp;
      rp.connectDown = cell.connectDown;
      rp.connectLeft = cell.connectLeft;
      rp.connectRight = cell.connectRight;
      data.rooms.push_back(rp);

      // Поиск спавна игрока в префабе
      if (!playerSpawnFound && rp.prefabIndex == 1) {
        const std::string &prefab =
            m_prefabs[rp.prefabIndex % m_prefabs.size()];
        for (int py = 0; py < roomSz && !playerSpawnFound; ++py)
          for (int px = 0; px < roomSz && !playerSpawnFound; ++px)
            if (prefab[py * roomSz + px] == '@') {
              data.playerSpawnPoint = sf::Vector2<float>(
                  (rp.tileX + px) * config::TILE_SIZE +
                      config::TILE_SIZE / 2.f,
                  (rp.tileY + py) * config::TILE_SIZE +
                      config::TILE_SIZE / 2.f);
              playerSpawnFound = true;
            }
      }
    }
  }

  // ── ЭТАП 2: коридоры ──
  for (int cy = 0; cy < m_config.gridHeight; ++cy) {
    for (int cx = 0; cx < m_config.gridWidth; ++cx) {
      const Cell &cell = m_cells[cellIndex(cx, cy)];
      if (!cell.active)
        continue;

      const int roomTileX = cx * stride;
      const int roomTileY = cy * stride;

      if (cell.connectRight && cx + 1 < m_config.gridWidth) {
        if (corrLen == 0)
          continue; // shared walls: комнаты сами открывают двери

        CorridorPlacement cp;
        cp.isHorizontal = true;
        cp.tileX = roomTileX + roomSz;
        cp.tileY = roomTileY + doorIdx - halfW - 1;
        cp.width = gapSize;
        cp.height = corrWid + 2;
        data.corridors.push_back(cp);
      }

      if (cell.connectDown && cy + 1 < m_config.gridHeight) {
        if (corrLen == 0)
          continue; // shared walls: комнаты сами открывают двери

        CorridorPlacement cp;
        cp.isHorizontal = false;
        cp.tileX = roomTileX + doorIdx - halfW - 1;
        cp.tileY = roomTileY + roomSz;
        cp.width = corrWid + 2;
        cp.height = gapSize;
        data.corridors.push_back(cp);
      }
    }
  }
}