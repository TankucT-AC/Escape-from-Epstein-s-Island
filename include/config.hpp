#ifndef CONFIG_PROJECT_HPP
#define CONFIG_PROJECT_HPP

#define DEBUG_DRAW_COLLISIONS 1
#define DEBUG_NOT_AUTH 1

#include <string>
#include <vector>

namespace config
{   
    static const std::string IPv4 = "127.0.0.1";
    static const int PORT = 8080;
    static const std::string SCHEME_HOST_PORT = "https://" + IPv4 + ":" + std::to_string(PORT);

    // Математические константы
    static const float PI = 3.14159265f;

    // Параметры для игрового окна
    static const int GAMEBOARD_HEIGHT       = 800;
    static const int GAMEBOARD_WIDTH        = 1200;
    static const int TILE_SIZE = 64;
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
}

#endif