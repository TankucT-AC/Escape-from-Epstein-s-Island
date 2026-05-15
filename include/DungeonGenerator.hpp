#ifndef DUNGEON_GENERATOR_HPP
#define DUNGEON_GENERATOR_HPP

#include "BSPTree.hpp"

struct DungeonData 
{
    MapData::RoomGrid grid;
    sf::Vector2<float> playerSpawnPoint;
};

class DungeonGenerator 
{
private:
    std::unique_ptr<BSPNode> m_root;
    std::mt19937 m_rng;

    void branchWorkspace(std::unique_ptr<BSPNode>& node, int currentDepth, int maxDepth, int minSize) {
        if (!node || currentDepth >= maxDepth) return;
        if (node->split(minSize, m_rng)) {
            branchWorkspace(node->leftChild, currentDepth + 1, maxDepth, minSize);
            branchWorkspace(node->rightChild, currentDepth + 1, maxDepth, minSize);
        }
    }

public:
    DungeonGenerator() {
        std::random_device rd;
        m_rng.seed(rd());
    }

    DungeonData generateDungeon(int width, int height, int maxDepth, int minNodeSize, int minRoomSize) 
    {
        m_root = std::make_unique<BSPNode>(sf::IntRect(0, 0, width, height));
        branchWorkspace(m_root, 0, maxDepth, minNodeSize);

        MapData::RoomGrid grid(height, std::vector<int>(width, 0));

        sf::Vector2<int> spawnTile = m_root->carve(grid, minRoomSize, m_rng);

        // ПОСТ-ПРОЦЕССИНГ СТЕН (твой текущий код обводки стен)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y][x] == 2) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            int ny = y + dy; int nx = x + dx;
                            if (ny >= 0 && ny < height && nx >= 0 && nx < width && grid[ny][nx] == 0) {
                                grid[ny][nx] = 1;
                            }
                        }
                    }
                }
            }
        }

        // Очистка маркеров пола (превращаем 2 в 0)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y][x] == 2) grid[y][x] = 0;
            }
        }

        // Конвертируем тайлы в пиксели. 
        // Добавляем +0.5f, чтобы персонаж встал ровно в центр тайла, а не на его верхний левый угол
        sf::Vector2<float> pixelSpawn(
            (static_cast<float>(spawnTile.x) + 0.5f) * config::TILE_SIZE,
            (static_cast<float>(spawnTile.y) + 0.5f) * config::TILE_SIZE
        );

        // Возвращаем структуру с сеткой и точкой спавна
        return { grid, pixelSpawn };
    }
};

#endif