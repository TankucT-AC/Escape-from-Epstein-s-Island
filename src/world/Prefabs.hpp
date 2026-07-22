// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PREFABS_HPP
#define PREFABS_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace Prefabs {

const int PREFAB_SIZE = 13;

const int DOOR_INDEX = 6;

// Именованные индексы префабов
constexpr uint8_t IDX_SPAWN = 0;
constexpr uint8_t IDX_COMBAT = 1;
constexpr uint8_t IDX_TREASURE = 2;
constexpr uint8_t IDX_ARENA = 3;
constexpr uint8_t IDX_PURIFICATION = 4;
constexpr uint8_t IDX_PORTAL_ROOM = 5;

// clang-format off
// Спавн игрока
inline const std::vector<std::string> SPAWN = {
    "######+######",
    "#V..........#",
    "#...........#",
    "#...........#",
    "#.....@.....#",
    "#...........#",
    "+....O.O....+",
    "#...........#",
    "#...........#",
    "#...........#",
    "#...........#",
    "#...........#",
    "######+######",
};

// Боевая комната — 4 спавнера врагов
inline const std::vector<std::string> COMBAT = {
    "######+######",
    "#...........#",
    "#...x...x...#",
    "#...........#",
    "#....O.O....#",
    "#...........#",
    "+...........+",
    "#...........#",
    "#....O.O....#",
    "#...........#",
    "#...x...x...#",
    "#...........#",
    "######+######",
};

// Сокровищница
inline const std::vector<std::string> TREASURE = {
    "######+######",
    "#...........#",
    "#...........#",
    "#....C.C....#",
    "#...........#",
    "#.....!.....#",
    "+...........+",
    "#.....!.....#",
    "#...........#",
    "#....C.C....#",
    "#...........#",
    "#...........#",
    "######+######",
};

// Арена — 4 спавнера врагов
inline const std::vector<std::string> ARENA = {
    "######+######",
    "#...........#",
    "#..x.....x..#",
    "#...........#",
    "#...........#",
    "#.....!.....#",
    "+....O.O....+",
    "#.....!.....#",
    "#...........#",
    "#...........#",
    "#..x.....x..#",
    "#...........#",
    "######+######",
};

// Чистилище — завершающая комната (нет дверей)
inline const std::vector<std::string> PURIFICATION = {
    "#############",
    "#############",
    "#############",
    "#############",
    "#############",
    "#############",
    "#############",
    "#############",
    "#############",
    "#############",
    "#############",
    "#############",
    "#############",
};

// Комната с порталом (центр пустой, портал спавнится отдельно)
inline const std::vector<std::string> PORTAL_ROOM = {
    "######+######",
    "#...........#",
    "#...........#",
    "#...........#",
    "#...........#",
    "#...........#",
    "+...........+",
    "#...........#",
    "#...........#",
    "#...........#",
    "#...........#",
    "#...........#",
    "######+######",
};

inline const std::vector<std::vector<std::string>> ALL = {
    SPAWN,
    COMBAT,
    TREASURE,
    ARENA,
    PURIFICATION,
    PORTAL_ROOM,
};
// clang-format on
} // namespace Prefabs

#endif
