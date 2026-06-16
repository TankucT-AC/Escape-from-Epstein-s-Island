// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BSP_TREE_HPP
#define BSP_TREE_HPP

#include "src/core/config.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <memory>
#include <random>

class BSPNode {
private:
  sf::IntRect area; // Область ноды в тайлах
  std::unique_ptr<BSPNode> leftChild;
  std::unique_ptr<BSPNode> rightChild;

  friend class DungeonGenerator;

public:
  explicit BSPNode(const sf::IntRect &tileArea);

  bool split(int minNodeSize, std::mt19937 &rng);

  sf::Vector2<int> carve(MapData::RoomGrid &grid, int minRoomSize,
                         std::mt19937 &rng);

  bool isLeaf() const { return !leftChild && !rightChild; }
};

#endif // BSP_TREE_HPP