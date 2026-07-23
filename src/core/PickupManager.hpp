// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PICKUPMANAGER_HPP
#define PICKUPMANAGER_HPP

#include "src/core/ResourceManager.hpp"
#include "src/game/Player.hpp"
#include "src/game/WeaponPickup.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

class LevelManager;

/**
 * @brief Менеджер подбираемых предметов.
 *
 * Владеет всеми WeaponPickup на уровне, обрабатывает открытие сундуков
 * (спавн нового оружия) и подбор/обмен оружия игроком.
 */
class PickupManager {
private:
  std::vector<std::unique_ptr<WeaponPickup>> m_pickups;

  /**
   * @brief Обменивает оружие из пикапа на активное оружие игрока.
   * @param player Игрок, чей инвентарь изменяется.
   * @param rm Менеджер ресурсов для текстуры выброшенного оружия.
   * @param pickup Пикап, из которого забирают оружие.
   * @return Пикап с выброшенным оружием игрока, либо nullptr, если обмена
   *         не было (свободный слот) или пикап пуст.
   */
  std::unique_ptr<WeaponPickup> swapWeapon(Player &player, ResourceManager &rm,
                                           WeaponPickup &pickup);

public:
  PickupManager() = default;

  /**
   * @brief Пытается подобрать/обменять оружие пикапом под курсором.
   *
   * Если у игрока есть свободный слот — оружие просто добавляется в инвентарь.
   * Если инвентарь полон — активное оружие выбрасывается на место пикапа.
   * @param player Игрок.
   * @param rm Менеджер ресурсов.
   * @param mousePos Позиция курсора в мировых координатах.
   */
  void trySwapWeapon(Player &player, ResourceManager &rm,
                     sf::Vector2<float> mousePos);

  /**
   * @brief Пытается открыть сундук под курсором и заспавнить оружие.
   * @param levelManager Менеджер уровня для поиска сундука.
   * @param rm Менеджер ресурсов.
   * @param worldPos Позиция курсора в мировых координатах.
   * @param round Текущий раунд (определяет выпадающее оружие).
   */
  void tryOpenChest(LevelManager &levelManager, ResourceManager &rm,
                    sf::Vector2<float> worldPos, int round);

  const std::vector<std::unique_ptr<WeaponPickup>> &getPickups() const {
    return m_pickups;
  }

  /** @brief Удаляет все пикапы (при смене раунда/чистилище). */
  void clear() { m_pickups.clear(); }
};

#endif
