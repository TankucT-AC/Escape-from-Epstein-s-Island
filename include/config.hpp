#ifndef CONFIG_PROJECT_HPP
#define CONFIG_PROJECT_HPP

#define DEBUG_DRAW_COLLISIONS 1
#define DEBUG_AUTH 0

#include <string>
#include <vector>

namespace config
{
    // Математические константы
    const float PI = 3.14159265f;

    // Параметры для игрового окна
    static const int GAMEBOARD_HEIGHT       = 800;
    static const int GAMEBOARD_WIDTH        = 1200;
    const int TILE_SIZE = 64;
    static const std::string GAMEBOARD_NAME = "gameboard";

    static const std::string ASSETS_PATH    = "assets/";
    static const std::string TEXTURE_PATH   = ASSETS_PATH + "textures/";

    // Пути для текстур
    static const std::string PLAYER_TEXTURE = TEXTURE_PATH + "player.png";
    static const std::string BULLET_PLAYER_TEXTURE = 
                                       TEXTURE_PATH + "player_bullet.png";
    static const std::string ENEMY_TEXTURE = TEXTURE_PATH +   "enemy.png";
    static const std::string DEFAULT_WALL_TEXTURE = TEXTURE_PATH + 
                                                       "default_wall.png";

    /*
        ХАРАКТЕРИСТИКИ ДЛЯ КЛАССОВ
    */

    // Bullet
    static const float BULLET_DEFAULT_LIFETIME = 2.f;
    static const float BULLET_DEFAULT_SPEED =  200.f;
    static const float BULLET_DEFAULT_DAMAGE = 0.5f;

    // Player
    static const float PLAYER_DEFAULT_SPEED =      300.f;
    static const float PLAYER_DEFAULT_SHOOT_DELAY = 0.4f;
    static const float PLAYER_DEFAULT_HEALTH =      10.f;

    // Enemy
    static const float ENEMY_DEFAULT_SPEED = 150.f;
    static const float ENEMY_DEFAULT_HEALTH = 5.f;
}

namespace MapData {
    using RoomGrid = std::vector<std::vector<int>>;

    // Обычная квадратная комната
    const RoomGrid SQUARE_ROOM = {
        {1, 1, 2, 1, 1},
        {1, 0, 0, 0, 1},
        {2, 0, 0, 0, 2},
        {1, 0, 0, 0, 1},
        {1, 1, 2, 1, 1}
    };

    // Г-образный коридор
    const RoomGrid L_CORRIDOR = {
        {1, 1, 2, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 1, 1, 1},
        {1, 0, 0, 0, 2},
        {1, 1, 1, 1, 1}
    };

    // Комната-крест (четыре выхода)
    const RoomGrid CROSS_ROOM = {
        {1, 1, 2, 1, 1},
        {1, 0, 0, 0, 1},
        {2, 0, 0, 0, 2},
        {1, 0, 0, 0, 1},
        {1, 1, 2, 1, 1}
    };
    
    // Длинный горизонтальный коридор
    const RoomGrid HORIZ_HALL = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 0, 0, 0, 0, 0, 0, 0, 2},
        {1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
}

#endif