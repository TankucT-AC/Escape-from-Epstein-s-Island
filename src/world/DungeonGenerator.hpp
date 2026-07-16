// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DUNGEON_GENERATOR_HPP
#define DUNGEON_GENERATOR_HPP

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string>
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
  int gridHeight = 5;
  int walkSteps = 0;
  int corridorLength = 0;
  int corridorWidth = 3;
  int prefabSize = 13;
  unsigned int seed = 0;
};

struct DungeonData {
  std::vector<std::vector<int>> grid;
  sf::Vector2<float> playerSpawnPoint;
  int roomCount = 0;
};

class DungeonGenerator {
public:
  explicit DungeonGenerator(const DungeonConfig &config);

  DungeonData generate();

  const std::vector<Cell> &getCells() const { return m_cells; }

  static DungeonConfig defaultConfig();

private:
  DungeonConfig m_config;
  std::vector<Cell> m_cells;
  std::vector<std::string> m_prefabs;

  void initPrefabs();
  void randomWalk();
  void resolveConnections();
  void assembleGrid(DungeonData &data);
  int cellIndex(int x, int y) const;
};

#endif