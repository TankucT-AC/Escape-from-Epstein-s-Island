// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DUNGEON_GENERATOR_HPP
#define DUNGEON_GENERATOR_HPP

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include <vector>

enum class Tile : char {
  Wall = '#',
  Floor = '.',
  Door = '+',
  Cover = 'O',
  EnemySpawn = 'x',
  PlayerSpawn = '@',
  Barrel = '!',
  VendingMachine = 'V',
  Chest = 'C',
  Forge = 'F',
};

struct Cell {
  int x = 0;
  int y = 0;
  bool active = false;
  bool connectUp = false;
  bool connectDown = false;
  bool connectLeft = false;
  bool connectRight = false;
  uint8_t prefabIndex = 0;
};

struct DungeonConfig {
  int gridWidth = 5;
  int gridHeight = 4;
  int walkSteps = 6;
  int corridorLength = 6;
  int corridorWidth = 3;
  int prefabSize = 13;
  unsigned int seed = 0;
};

// Данные для размещения одной комнаты
struct RoomPlacement {
  uint8_t prefabIndex = 0;
  int tileX = 0;
  int tileY = 0;
  bool connectUp = false;
  bool connectDown = false;
  bool connectLeft = false;
  bool connectRight = false;
};

// Данные для размещения одного коридора
struct CorridorPlacement {
  bool isHorizontal = true;
  int tileX = 0;
  int tileY = 0;
  int width = 0;  // полная ширина в тайлах (включая стенки)
  int height = 0; // полная высота в тайлах (включая стенки)
};

struct DungeonData {
  std::vector<RoomPlacement> rooms;
  std::vector<CorridorPlacement> corridors;
  sf::Vector2<float> playerSpawnPoint;
  int corridorWidth = 3;
};

class DungeonGenerator {
public:
  explicit DungeonGenerator(const DungeonConfig &config);

  DungeonData generate();
  void reseed() { m_config.seed = std::random_device{}(); }

  const std::vector<Cell> &getCells() const { return m_cells; }

  static DungeonConfig defaultConfig();

private:
  DungeonConfig m_config;
  std::vector<Cell> m_cells;
  std::vector<std::pair<int, int>> m_walkPath;
  std::vector<std::string> m_prefabs;

  void initPrefabs();
  void randomWalk(unsigned int seed);
  void resolveConnections();
  void assembleData(DungeonData &data);
  int cellIndex(int x, int y) const;
};

#endif