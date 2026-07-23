// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CONFIG_PROJECT_HPP
#define CONFIG_PROJECT_HPP

#define DEBUG_DRAW_COLLISIONS 0
#define DEBUG_NOT_AUTH 1

#include <string>

namespace config {
static const std::string IPv4 = "127.0.0.1";
static const int PORT = 8080;
static const std::string SCHEME_HOST_PORT =
    "https://" + IPv4 + ":" + std::to_string(PORT);

static const float PI = 3.14159265f;

static const int GAMEBOARD_HEIGHT = 800;
static const int GAMEBOARD_WIDTH = 1200;
static const int TILE_SIZE = 64;
static const std::string GAMEBOARD_NAME = "gameboard";

static const std::string ASSETS_PATH = "assets/";
static const std::string TEXTURE_PATH = ASSETS_PATH + "textures/";

// ─── Игрок ───
static const std::string PLAYER_SPRITESHEET =
    TEXTURE_PATH + "player/knight.png";
static const float PLAYER_SCALE = 2.5f;
static const int PLAYER_FRAME_W = 40;
static const int PLAYER_FRAME_H = 40;
static const int PLAYER_FRAME_COUNT = 4;
static const float PLAYER_FRAME_TIME = 0.08f;
static const int PLAYER_IDLE_ROW = 0;
static const int PLAYER_RUN_ROW = 1;
static const float PLAYER_WEAPON_Y_OFFSET = 26.f;
static const float PLAYER_HITBOX_Y_OFFSET = 20.f;
static const float PLAYER_HITBOX_HALF_W = 12.f;
static const float PLAYER_HITBOX_HALF_H = 8.f;
static const float PLAYER_HITBOX_W = 24.f;
static const float PLAYER_HITBOX_H = 16.f;

static const float PLAYER_DEFAULT_SPEED = 300.f;
static const float PLAYER_DEFAULT_SHOOT_DELAY = 0.4f;
static const float PLAYER_DEFAULT_HEALTH = 10.f;

// ─── Враги ───
static const std::string ENEMY_SPRITESHEET =
    TEXTURE_PATH + "enemies/small_enemies.png";
static const float ENEMY_SCALE = 2.f;
static const int ENEMY_FRAME_W = 32;
static const int ENEMY_FRAME_H = 32;
static const int ENEMY_FRAME_COUNT = 4;
static const float ENEMY_FRAME_TIME = 0.15f;

static const float ENEMY_DEFAULT_SPEED = 150.f;
static const float ENEMY_DEFAULT_HEALTH = 5.f;

// ─── Портал ───
static const std::string PORTAL_SPRITESHEET =
    TEXTURE_PATH + "world/portal_states.png";
static const int PORTAL_FRAME_W = 212;
static const int PORTAL_FRAME_H = 208;
static const int PORTAL_FRAME_COUNT = 8;
static const float PORTAL_FRAME_TIME = 0.15f;
static const float PORTAL_SCALE = 1.5f;
static const float PORTAL_INACTIVE_SCALE = 0.25f;

// ─── Пули ───
static const float BULLET_SCALE = 2.f;
static const float BULLET_HITBOX_W = 12.f;
static const float BULLET_HITBOX_H = 4.f;
static const float BULLET_DEFAULT_LIFETIME = 2.f;
static const float BULLET_DEFAULT_SPEED = 200.f;
static const float BULLET_DEFAULT_DAMAGE = 0.5f;

// ─── Оружие ───
static const std::string WEAPON_01_TEX =
    TEXTURE_PATH + "weapon/weapon_01/weapon_01.png";
static const std::string WEAPON_01_BULLET =
    TEXTURE_PATH + "weapon/weapon_01/weapon_bullet_01.png";
static const float WEAPON_01_FIRE_RATE = 0.4f;
static const float WEAPON_01_DAMAGE = 10.f;
static const float WEAPON_01_BULLET_SPEED = 300.f;
static const float WEAPON_01_SCALE = 2.2f; // пистолет 26x8 -> 57x18 px

static const std::string WEAPON_02_TEX =
    TEXTURE_PATH + "weapon/weapon_02/weapon_02.png";
static const std::string WEAPON_02_BULLET =
    TEXTURE_PATH + "weapon/weapon_02/weapon_bullet_02.png";
static const float WEAPON_02_FIRE_RATE = 0.6f;
static const float WEAPON_02_DAMAGE = 1.5f;
static const float WEAPON_02_BULLET_SPEED = 250.f;
static const float WEAPON_02_SCALE = 2.0f; // винтовка 38x15 -> 76x30 px

static const std::string WEAPON_03_TEX =
    TEXTURE_PATH + "weapon/weapon_03/weapon_03.png";
static const std::string WEAPON_03_BULLET =
    TEXTURE_PATH + "weapon/weapon_03/weapon_bullet_03.png";
static const float WEAPON_03_FIRE_RATE = 0.15f;
static const float WEAPON_03_DAMAGE = 0.3f;
static const float WEAPON_03_BULLET_SPEED = 400.f;
static const float WEAPON_03_SCALE = 2.4f; // ПП 21x11 -> 50x26 px

static const int MAX_WEAPON_SLOTS = 2;
static const float MELEE_FIRE_RATE = 0.5f;

// ─── Отладка ───
static const float DEBUG_OUTLINE_THICKNESS = 1.f;
static const float DEBUG_PORTAL_OUTLINE = 2.f;

// ─── Стены/пол ───
static const std::string DEFAULT_WALL_TEXTURE =
    TEXTURE_PATH + "default_wall.png";
static const std::string DESERT_FLOOR_TEXTURE =
    TEXTURE_PATH + "default_floor.png";

// ─── Сундуки ───
static const std::string DEFAUTL_CHEST_CLOSED_TEXTURE =
    TEXTURE_PATH + "default_chest_closed.png";
static const std::string DEFAUTL_CHEST_OPEN_TEXTURE =
    TEXTURE_PATH + "default_chest_open.png";

// ─── Ворота ───
static const std::string DEFAULT_GATE_CLOSED =
    TEXTURE_PATH + "default_gate_closed.jpg";
static const std::string DEFAUTL_GATE_OPEN =
    TEXTURE_PATH + "default_gate_open.jpg";

// Deprecated
static const std::string BULLET_PLAYER_TEXTURE =
    TEXTURE_PATH + "player_bullet.png";
static const std::string ENEMY_TEXTURE = TEXTURE_PATH + "enemy.png";
static const std::string PLAYER_TEXTURE = TEXTURE_PATH + "player.png";
} // namespace config

#endif
