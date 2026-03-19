#ifndef CONFIG_PROJECT_HPP
#define CONFIG_PROJECT_HPP

#include <string>

namespace config
{
    // Параметры для игрового окна
    static const int GAMEBOARD_HEIGHT       = 800;
    static const int GAMEBOARD_WIDTH        = 1200;
    static const std::string GAMEBOARD_NAME = "gameboard";

    static const std::string ASSETS_PATH    = "assets/";
    static const std::string TEXTURE_PATH   = ASSETS_PATH + "textures/";

    // Пути для текстур
    static const std::string PLAYER_TEXTURE = TEXTURE_PATH + "player.png";
    static const std::string BULLET_PLAYER_TEXTURE = 
                                       TEXTURE_PATH + "player_bullet.png";
    static const std::string ENEMY_TEXTURE = TEXTURE_PATH +   "enemy.png";

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

#endif