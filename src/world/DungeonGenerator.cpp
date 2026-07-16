// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DungeonGenerator.hpp"
#include "Prefabs.hpp"
#include "src/core/config.hpp"
#include <algorithm>
#include <random>

DungeonGenerator::DungeonGenerator(const DungeonConfig &config)
    : m_config(config) {
  if (m_config.seed == 0)
    m_config.seed = std::random_device{}();
  initPrefabs();
}

DungeonConfig DungeonGenerator::defaultConfig() {
  DungeonConfig cfg;
  cfg.gridWidth = 5;
  cfg.gridHeight = 5;
  cfg.walkSteps = 0;
  cfg.corridorLength = 0;
  cfg.corridorWidth = 3;
  cfg.prefabSize = 13;
  cfg.seed = 0;
  return cfg;
}

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
    }

  randomWalk();
  resolveConnections();

  DungeonData data;
  assembleGrid(data);
  return data;
}

int DungeonGenerator::cellIndex(int x, int y) const {
  return y * m_config.gridWidth + x;
}

void DungeonGenerator::randomWalk() {
  std::mt19937 rng(m_config.seed);
  std::uniform_int_distribution<int> xDist(0, m_config.gridWidth - 1);
  std::uniform_int_distribution<int> yDist(0, m_config.gridHeight - 1);
  std::uniform_int_distribution<uint8_t> prefabDist(
      2, static_cast<uint8_t>(Prefabs::ALL.size() - 1));

  std::vector<std::vector<bool>> visited(
      m_config.gridHeight,
      std::vector<bool>(m_config.gridWidth, false));

  int startX = xDist(rng);
  int startY = yDist(rng);
  visited[startY][startX] = true;
  m_cells[cellIndex(startX, startY)].active = true;
  m_cells[cellIndex(startX, startY)].prefabIndex = 1;

  std::vector<std::pair<int, int>> stack;
  stack.push_back({startX, startY});

  const int dx[4] = {0, 0, -1, 1};
  const int dy[4] = {-1, 1, 0, 0};

  while (!stack.empty()) {
    auto [cx, cy] = stack.back();

    int dirs[4] = {0, 1, 2, 3};
    std::shuffle(std::begin(dirs), std::end(dirs), rng);

    bool found = false;
    for (int d : dirs) {
      int nx = cx + dx[d];
      int ny = cy + dy[d];

      if (nx >= 0 && nx < m_config.gridWidth && ny >= 0 &&
          ny < m_config.gridHeight && !visited[ny][nx]) {
        visited[ny][nx] = true;
        m_cells[cellIndex(nx, ny)].active = true;
        m_cells[cellIndex(nx, ny)].prefabIndex = prefabDist(rng);
        stack.push_back({nx, ny});
        found = true;
        break;
      }
    }

    if (!found)
      stack.pop_back();
  }
}

void DungeonGenerator::resolveConnections() {
  for (int y = 0; y < m_config.gridHeight; ++y) {
    for (int x = 0; x < m_config.gridWidth; ++x) {
      Cell &cell = m_cells[cellIndex(x, y)];
      if (!cell.active)
        continue;

      if (y > 0 && m_cells[cellIndex(x, y - 1)].active) {
        cell.connectUp = true;
        m_cells[cellIndex(x, y - 1)].connectDown = true;
      }
      if (y < m_config.gridHeight - 1 && m_cells[cellIndex(x, y + 1)].active) {
        cell.connectDown = true;
        m_cells[cellIndex(x, y + 1)].connectUp = true;
      }
      if (x > 0 && m_cells[cellIndex(x - 1, y)].active) {
        cell.connectLeft = true;
        m_cells[cellIndex(x - 1, y)].connectRight = true;
      }
      if (x < m_config.gridWidth - 1 && m_cells[cellIndex(x + 1, y)].active) {
        cell.connectRight = true;
        m_cells[cellIndex(x + 1, y)].connectLeft = true;
      }
    }
  }
}

void DungeonGenerator::assembleGrid(DungeonData &data) {
  const int stride = m_config.prefabSize + m_config.corridorLength;
  const bool sharedWalls = (m_config.corridorLength == 0);
  const int actualStride = sharedWalls ? m_config.prefabSize - 1 : stride;
  const int totalWidth =
      (m_config.gridWidth - 1) * actualStride + m_config.prefabSize;
  const int totalHeight =
      (m_config.gridHeight - 1) * actualStride + m_config.prefabSize;

  std::vector<std::vector<int>> tileGrid(totalHeight,
                                         std::vector<int>(totalWidth, 1));

  bool playerSpawnFound = false;

  for (int cy = 0; cy < m_config.gridHeight; ++cy) {
    for (int cx = 0; cx < m_config.gridWidth; ++cx) {
      const Cell &cell = m_cells[cellIndex(cx, cy)];
      if (!cell.active)
        continue;

      const int roomTileX = cx * actualStride;
      const int roomTileY = cy * actualStride;
      const std::string &prefab =
          m_prefabs[cell.prefabIndex % m_prefabs.size()];

      for (int py = 0; py < m_config.prefabSize; ++py) {
        for (int px = 0; px < m_config.prefabSize; ++px) {
          const int globalX = roomTileX + px;
          const int globalY = roomTileY + py;

          if (globalX < 0 || globalX >= totalWidth || globalY < 0 ||
              globalY >= totalHeight)
            continue;

          char tileChar = prefab[py * m_config.prefabSize + px];
          int tileVal = 1;

          switch (tileChar) {
          case '#':
          case 'O':
            tileVal = 1;
            break;
          case '.':
          case 'x':
          case '!':
          case 'V':
          case 'C':
          case 'F':
            tileVal = 0;
            break;
          case '+': {
            bool connected = false;
            if (py == 0)
              connected = cell.connectUp;
            else if (py == m_config.prefabSize - 1)
              connected = cell.connectDown;
            else if (px == 0)
              connected = cell.connectLeft;
            else
              connected = cell.connectRight;
            tileVal = connected ? 0 : 1;
            break;
          }
          case '@':
            tileVal = 0;
            if (!playerSpawnFound) {
              data.playerSpawnPoint = sf::Vector2<float>(
                  globalX * config::TILE_SIZE + config::TILE_SIZE / 2.f,
                  globalY * config::TILE_SIZE + config::TILE_SIZE / 2.f);
              playerSpawnFound = true;
            }
            break;
          default:
            tileVal = 0;
            break;
          }

          tileGrid[globalY][globalX] = tileVal;
        }
      }
    }
  }

  const int halfW = m_config.corridorWidth / 2;

  for (int cy = 0; cy < m_config.gridHeight; ++cy) {
    for (int cx = 0; cx < m_config.gridWidth; ++cx) {
      const Cell &cell = m_cells[cellIndex(cx, cy)];
      if (!cell.active)
        continue;

      const int roomTileX = cx * actualStride;
      const int roomTileY = cy * actualStride;

      if (cell.connectRight && cx + 1 < m_config.gridWidth) {
        const int boundaryX = roomTileX + m_config.prefabSize - 1;
        const int doorY = roomTileY + Prefabs::DOOR_INDEX;
        if (sharedWalls) {
          for (int ty = doorY - halfW; ty <= doorY + halfW; ++ty)
            if (ty >= 0 && ty < totalHeight)
              tileGrid[ty][boundaryX] = 0;
        } else {
          const int nextRoomX = (cx + 1) * stride;
          const int startX = roomTileX + m_config.prefabSize;
          const int endX = nextRoomX;
          for (int tx = startX; tx < endX; ++tx)
            for (int ty = doorY - halfW; ty <= doorY + halfW; ++ty)
              if (tx >= 0 && tx < totalWidth && ty >= 0 && ty < totalHeight)
                tileGrid[ty][tx] = 0;
        }
      }

      if (cell.connectDown && cy + 1 < m_config.gridHeight) {
        const int boundaryY = roomTileY + m_config.prefabSize - 1;
        const int doorX = roomTileX + Prefabs::DOOR_INDEX;
        if (sharedWalls) {
          for (int tx = doorX - halfW; tx <= doorX + halfW; ++tx)
            if (tx >= 0 && tx < totalWidth)
              tileGrid[boundaryY][tx] = 0;
        } else {
          const int nextRoomY = (cy + 1) * stride;
          const int startY = roomTileY + m_config.prefabSize;
          const int endY = nextRoomY;
          for (int ty = startY; ty < endY; ++ty)
            for (int tx = doorX - halfW; tx <= doorX + halfW; ++tx)
              if (tx >= 0 && tx < totalWidth && ty >= 0 && ty < totalHeight)
                tileGrid[ty][tx] = 0;
        }
      }
    }
  }

  data.grid = std::move(tileGrid);
  data.roomCount = 0;
  for (const auto &cell : m_cells)
    if (cell.active)
      data.roomCount++;
}