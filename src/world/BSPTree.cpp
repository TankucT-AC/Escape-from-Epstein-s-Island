// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BSPTree.hpp"
#include "Room.hpp"

BSPNode::BSPNode(const sf::IntRect &tileArea)
    : area(tileArea), leftChild(nullptr), rightChild(nullptr) {}

bool BSPNode::split(int minNodeSize, std::mt19937 &rng) {
  if (!isLeaf())
    return false;

  bool splitHorizontal = rng() % 2 == 0;
  if (area.width > area.height &&
      static_cast<float>(area.width) / area.height >= 1.25f)
    splitHorizontal = false;
  else if (area.height > area.width &&
           static_cast<float>(area.height) / area.width >= 1.25f)
    splitHorizontal = true;

  int totalSize = splitHorizontal ? area.height : area.width;
  int maxSplit = totalSize - minNodeSize;
  if (maxSplit <= minNodeSize)
    return false;

  std::uniform_int_distribution<int> dist(minNodeSize, maxSplit);
  int splitPoint = dist(rng);

  if (splitHorizontal) {
    leftChild = std::make_unique<BSPNode>(
        sf::IntRect(area.left, area.top, area.width, splitPoint));
    rightChild = std::make_unique<BSPNode>(
        sf::IntRect(area.left, area.top + splitPoint, area.width,
                    area.height - splitPoint));
  } else {
    leftChild = std::make_unique<BSPNode>(
        sf::IntRect(area.left, area.top, splitPoint, area.height));
    rightChild = std::make_unique<BSPNode>(
        sf::IntRect(area.left + splitPoint, area.top, area.width - splitPoint,
                    area.height));
  }
  return true;
}

sf::Vector2<int> BSPNode::carve(MapData::RoomGrid &grid, int minRoomSize,
                                std::mt19937 &rng) {
  if (!isLeaf()) {
    sf::Vector2<int> centerL = leftChild->carve(grid, minRoomSize, rng);
    sf::Vector2<int> centerR = rightChild->carve(grid, minRoomSize, rng);

    int startX = std::min(centerL.x, centerR.x);
    int endX = std::max(centerL.x, centerR.x);
    for (int x = startX; x <= endX; ++x) {
      grid[centerL.y][x] = static_cast<int>(RoomElements::TEMP_FLAG);
      if (centerL.y + 1 < static_cast<int>(grid.size()))
        grid[centerL.y + 1][x] = static_cast<int>(RoomElements::TEMP_FLAG);
    }

    int startY = std::min(centerL.y, centerR.y);
    int endY = std::max(centerL.y, centerR.y);
    for (int y = startY; y <= endY; ++y) {
      grid[y][centerR.x] = static_cast<int>(RoomElements::TEMP_FLAG);
      if (centerR.x + 1 < static_cast<int>(grid[0].size()))
        grid[y][centerR.x + 1] = static_cast<int>(RoomElements::TEMP_FLAG);
    }

    return centerL;
  }

  const int margin = 2;
  
  int availableW = area.width - 2 * margin;
  int availableH = area.height - 2 * margin;
  
  if (availableW < minRoomSize || availableH < minRoomSize) {
    return sf::Vector2<int>(area.left + area.width / 2, area.top + area.height / 2);
  }
  
  std::uniform_int_distribution<int> distW(
      std::max(minRoomSize, static_cast<int>(availableW * 0.7f)),
      std::max(minRoomSize, static_cast<int>(availableW * 0.9f))
  );
  std::uniform_int_distribution<int> distH(
      std::max(minRoomSize, static_cast<int>(availableH * 0.7f)),
      std::max(minRoomSize, static_cast<int>(availableH * 0.9f))
  );
  
  int roomW = distW(rng);
  int roomH = distH(rng);
  
  int roomLeft = area.left + margin + (availableW - roomW) / 2;
  int roomTop = area.top + margin + (availableH - roomH) / 2;
  
  for (int y = roomTop; y < roomTop + roomH; ++y) {
    for (int x = roomLeft; x < roomLeft + roomW; ++x) {
      grid[y][x] = static_cast<int>(RoomElements::TEMP_FLAG);
    }
  }
  
  return sf::Vector2<int>(roomLeft + roomW / 2, roomTop + roomH / 2);
}
