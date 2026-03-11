#ifndef CONFIG_PROJECT_HPP
#define CONFIG_PROJECT_HPP

#include <string>

namespace config
{
    static const int GAMEBOARD_HEIGHT       = 800;
    static const int GAMEBOARD_WIDTH        = 1200;
    static const std::string GAMEBOARD_NAME = "gameboard";

    static const std::string ASSETS_PATH    = "assets/";
    static const std::string TEXTURE_PATH   = ASSETS_PATH + "textures/";

    // Пути для текстур
    static const std::string PLAYER_TEXTURE = TEXTURE_PATH + "player.png";
    static const std::string BULLET_PLAYER_TEXTURE = 
                                       TEXTURE_PATH + "player_bullet.png";
}

#endif