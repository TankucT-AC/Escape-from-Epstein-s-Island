#include "BSPTree.hpp"

BSPNode::BSPNode(const sf::IntRect& tileArea) : area(tileArea), leftChild(nullptr), rightChild(nullptr) {}

bool BSPNode::split(int minNodeSize, std::mt19937& rng)
{
    if (!isLeaf()) return false;

    bool splitHorizontal = rng() % 2 == 0;
    if (area.width > area.height && static_cast<float>(area.width) / area.height >= 1.25f) splitHorizontal = false;
    else if (area.height > area.width && static_cast<float>(area.height) / area.width >= 1.25f) splitHorizontal = true;

    int maxSplit = (splitHorizontal ? area.height : area.width) - minNodeSize;
    if (maxSplit <= minNodeSize) return false; 

    std::uniform_int_distribution<int> dist(minNodeSize, maxSplit);
    int splitPoint = dist(rng);

    if (splitHorizontal) {
        leftChild = std::make_unique<BSPNode>(sf::IntRect(area.left, area.top, area.width, splitPoint));
        rightChild = std::make_unique<BSPNode>(sf::IntRect(area.left, area.top + splitPoint, area.width, area.height - splitPoint));
    } else {
        leftChild = std::make_unique<BSPNode>(sf::IntRect(area.left, area.top, splitPoint, area.height));
        rightChild = std::make_unique<BSPNode>(sf::IntRect(area.left + splitPoint, area.top, area.width - splitPoint, area.height));
    }
    return true;
}

sf::Vector2<int> BSPNode::carve(MapData::RoomGrid& grid, int minRoomSize, std::mt19937& rng)
{
    if (!isLeaf()) 
    {
        sf::Vector2<int> centerL = leftChild->carve(grid, minRoomSize, rng);
        sf::Vector2<int> centerR = rightChild->carve(grid, minRoomSize, rng);

        int startX = std::min(centerL.x, centerR.x);
        int endX = std::max(centerL.x, centerR.x);
        for (int x = startX; x <= endX; ++x) grid[centerL.y][x] = 2;

        int startY = std::min(centerL.y, centerR.y);
        int endY = std::max(centerL.y, centerR.y);
        for (int y = startY; y <= endY; ++y) grid[y][centerR.x] = 2;

        return centerL;
    }


    std::uniform_int_distribution<int> distW(minRoomSize, area.width - 2);
    std::uniform_int_distribution<int> distH(minRoomSize, area.height - 2);
    int roomW = distW(rng);
    int roomH = distH(rng);

    std::uniform_int_distribution<int> posX(1, area.width - roomW - 1);
    std::uniform_int_distribution<int> posY(1, area.height - roomH - 1);
    int roomLeft = area.left + posX(rng);
    int roomTop = area.top + posY(rng);

    for (int y = roomTop; y < roomTop + roomH; ++y) 
    {
        for (int x = roomLeft; x < roomLeft + roomW; ++x) 
        {
            grid[y][x] = 2; 
        }
    }

    return sf::Vector2<int>(roomLeft + roomW / 2, roomTop + roomH / 2);
}