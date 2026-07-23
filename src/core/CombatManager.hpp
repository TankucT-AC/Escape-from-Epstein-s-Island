// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef COMBATMANAGER_HPP
#define COMBATMANAGER_HPP

#include "ResourceManager.hpp"
#include "src/game/Enemy.hpp"
#include "src/game/Player.hpp"
#include "src/world/Room.hpp"
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

class LevelManager;

/**
 * @brief Менеджер боевых состояний.
 *
 * Управляет активацией боевых комнат, закрытием ворот, спавном
 * трёх волн врагов, завершением боя и наградой (сундук).
 * Также отслеживает перемещение игрока между комнатами для
 * автоматической активации боёв.
 */
class CombatManager final {
private:
  std::optional<std::reference_wrapper<Room>> m_activeRoom;
  std::optional<std::reference_wrapper<Room>> m_previousRoom;
  int m_combatWave = 0;
  bool m_gatesClosed = false;
  int m_maxWaves = 3;

public:
  CombatManager() = default;

  /** @brief Устанавливает количество волн на текущий раунд. */
  void setMaxWaves(int waves) { m_maxWaves = waves; }

  /** @brief Активирует боевую комнату (или авто-зачищает не-боевые). */
  void activate(Room &room);

  /** @brief Пытается закрыть ворота (если игрок не перекрывает их). */
  bool tryCloseGates(Room &room, const Player &player);

  /** @brief Спавнит новую волну врагов. */
  void spawnWave(Room &room, std::vector<std::unique_ptr<Enemy>> &enemies,
                 ResourceManager &rm);

  /** @brief Завершает бой в комнате: открывает ворота, сбрасывает состояние. */
  void clearRoom(Room &room);

  /** @brief Проверяет, уничтожены ли все враги текущей волны. Очищает вектор
   * при успехе. */
  bool checkWaveComplete(std::vector<std::unique_ptr<Enemy>> &enemies);

  /**
   * @brief Отслеживает переход игрока в новую комнату и автоматически
   * активирует бой при входе в неактивную Combat-комнату.
   * @param levelManager Менеджер уровня для поиска комнаты.
   * @param player Игрок (определение позиции).
   */
  void updatePlayerRoom(LevelManager &levelManager, const Player &player);

  /**
   * @brief Проверяет завершение волны и, при необходимости,
   * спавнит следующую или зачищает комнату с наградой.
   * @param enemies Вектор врагов (может очищаться волной/зачисткой).
   * @param rm Менеджер ресурсов для спавна сундука.
   */
  void tryCompleteWave(std::vector<std::unique_ptr<Enemy>> &enemies,
                       ResourceManager &rm);

  /** @brief Сбрасывает всё боевое состояние (при смене раунда). */
  void reset() {
    m_activeRoom.reset();
    m_previousRoom.reset();
    m_combatWave = 0;
    m_gatesClosed = false;
  }

  bool hasActiveRoom() const { return m_activeRoom.has_value(); }
  bool isSpawnPending() const { return hasActiveRoom() && !m_gatesClosed; }
  Room &activeRoom() { return m_activeRoom.value().get(); }
  int currentWave() const { return m_combatWave; }
  int maxWaves() const { return m_maxWaves; }
};

#endif
