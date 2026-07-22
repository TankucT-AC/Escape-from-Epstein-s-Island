// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/core/Engine.hpp"
#include "config.hpp"
#include "src/core/InputManager.hpp"
#include "src/game/Bullet.hpp"
#include "src/game/WeaponPickup.hpp"
#include "src/world/Chest.hpp"
#include "src/world/DungeonGenerator.hpp"
#include "src/world/Prefabs.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <memory>
#include <optional>

static std::unique_ptr<Weapon> makeWeapon(int idx) {
  if (idx == 2)
    return std::make_unique<RangedWeapon>(
        config::WEAPON_02_TEX, config::WEAPON_02_BULLET,
        config::WEAPON_02_FIRE_RATE, config::WEAPON_02_DAMAGE,
        config::WEAPON_02_BULLET_SPEED, config::WEAPON_02_SCALE);
  if (idx == 3)
    return std::make_unique<RangedWeapon>(
        config::WEAPON_03_TEX, config::WEAPON_03_BULLET,
        config::WEAPON_03_FIRE_RATE, config::WEAPON_03_DAMAGE,
        config::WEAPON_03_BULLET_SPEED, config::WEAPON_03_SCALE);
  return std::make_unique<RangedWeapon>(
      config::WEAPON_01_TEX, config::WEAPON_01_BULLET,
      config::WEAPON_01_FIRE_RATE, config::WEAPON_01_DAMAGE,
      config::WEAPON_01_BULLET_SPEED, config::WEAPON_01_SCALE);
}

static const std::string &weaponTexPath(int idx) {
  if (idx == 2)
    return config::WEAPON_02_TEX;
  if (idx == 3)
    return config::WEAPON_03_TEX;
  return config::WEAPON_01_TEX;
}

static bool roomsCleared(const std::vector<std::unique_ptr<Room>> &rooms) {
  for (const auto &r : rooms)
    if (r->getRoomType() == RoomType::Combat &&
        r->getCombatState() != CombatState::Cleared)
      return false;
  return true;
}

Engine::Engine()
    : player(resourceManager.getTexture(config::PLAYER_SPRITESHEET),
             sf::Vector2<float>{0.f, 0.f}),
      EngineVideoMode(config::GAMEBOARD_WIDTH, config::GAMEBOARD_HEIGHT),
      EngineWindow(std::make_unique<sf::RenderWindow>(EngineVideoMode,
                                                      config::GAMEBOARD_NAME)),
      EngineRender(*EngineWindow),
      dungeonGenerator(DungeonGenerator::defaultConfig()) {
  player.addWeapon(std::make_unique<RangedWeapon>(
                       config::WEAPON_01_TEX, config::WEAPON_01_BULLET,
                       config::WEAPON_01_FIRE_RATE, config::WEAPON_01_DAMAGE,
                       config::WEAPON_01_BULLET_SPEED, config::WEAPON_01_SCALE),
                   resourceManager);
  generateRound();
  EngineCamera.setCenter(player.getPosition());
}

void Engine::generateRound() {
  enemies.clear();
  bullets.clear();
  m_weaponPickups.clear();
  m_previousRoom.reset();
  m_portalRoom.reset();
  m_portal.reset();

  dungeonGenerator.reseed();
  DungeonData dungeon = dungeonGenerator.generate();
  levelManager.buildFromData(dungeon, resourceManager);
  player.setPosition(dungeon.playerSpawnPoint);
  player.initAnimation(config::PLAYER_FRAME_W, config::PLAYER_FRAME_H,
                       config::PLAYER_FRAME_COUNT, config::PLAYER_FRAME_TIME,
                       config::PLAYER_IDLE_ROW, config::PLAYER_RUN_ROW);
  player.getSprite().setScale(
      {config::PLAYER_SCALE, config::PLAYER_SCALE});

  for (const auto &r : levelManager.getRooms()) {
    if (r->getRoomType() == RoomType::Portal) {
      m_portalRoom = std::ref(*r);
      break;
    }
  }
}

void Engine::tryActivateCombat(Room &room) {
  if (room.getCombatState() != CombatState::Inactive)
    return;
  combatManager.activate(room);
}

void Engine::tryCompleteCombatWave() {
  if (!combatManager.hasActiveRoom())
    return;

  Room &room = combatManager.activeRoom();
  if (room.getCombatState() != CombatState::Active)
    return;

  if (!combatManager.checkWaveComplete(enemies))
    return;

  if (combatManager.currentWave() < combatManager.maxWaves()) {
    combatManager.spawnWave(room, enemies, resourceManager);
  } else {
    combatManager.clearRoom(room);
    room.spawnChest(resourceManager);
  }
}

void Engine::spawnPortalInRoom(Room &room) {
  m_portal = std::make_unique<Portal>(
      resourceManager.getTexture(config::PORTAL_SPRITESHEET),
      room.getChestPos());
  m_portal->activate();
}

std::unique_ptr<WeaponPickup> Engine::swapWeapon(WeaponPickup &pickup) {
  std::unique_ptr<Weapon> newWeapon = pickup.takeWeapon();
  if (!newWeapon)
    return nullptr;

  if (player.getWeaponCount() < static_cast<size_t>(config::MAX_WEAPON_SLOTS)) {
    player.addWeapon(std::move(newWeapon), resourceManager);
    return nullptr;
  }

  int oldSlot = player.activeWeaponIndex();
  std::unique_ptr<Weapon> oldWeapon = player.removeWeapon(oldSlot);
  player.addWeapon(std::move(newWeapon), resourceManager);
  player.switchWeapon(player.getWeaponCount() - 1, resourceManager);

  if (oldWeapon) {
    return std::make_unique<WeaponPickup>(
        resourceManager.getTexture(oldWeapon->getWeaponTexPath()),
        pickup.getPosition(), std::move(oldWeapon));
  }
  return nullptr;
}

void Engine::tryOpenChest(sf::Vector2<float> worldPos) {
  std::optional<std::reference_wrapper<Chest>> opt =
      levelManager.findChestAt(worldPos);
  if (!opt.has_value())
    return;
  Chest &chest = opt.value().get();
  if (chest.isOpen())
    return;

  chest.open();
  int wIdx = 1 + (m_round % 3);
  chest.setWeaponDrop(wIdx);
  m_weaponPickups.push_back(std::make_unique<WeaponPickup>(
      resourceManager.getTexture(weaponTexPath(wIdx)), chest.getPosition(),
      makeWeapon(wIdx)));
}

void Engine::tryAdvanceRound(sf::Vector2<float> worldPos) {
  if (!m_portal || !m_portal->isActive())
    return;
  if (!m_portal->getHitbox().contains(worldPos))
    return;

  m_round++;
  if (m_round > MAX_ROUNDS) {
    goToPurgatory();
  } else {
    generateRound();
  }
}

void Engine::goToPurgatory() {
  RoomPlacement rp;
  rp.prefabIndex = Prefabs::IDX_PURIFICATION;
  DungeonData purgData;
  purgData.rooms.push_back(rp);
  purgData.corridorWidth = 1;
  levelManager.buildFromData(purgData, resourceManager);
  m_weaponPickups.clear();
  enemies.clear();
  bullets.clear();
  // Центр комнаты 13x13 тайлов
  float center = config::TILE_SIZE * Prefabs::PREFAB_SIZE / 2.f;
  player.setPosition({center, center});
  m_portal.reset();
}

void Engine::run() {
  while (EngineWindow->isOpen()) {
    sf::Time dt = EngineClock.restart();
    this->update(dt);
    this->render();
  }
}

void Engine::render() {
  EngineRender.setCamera(EngineCamera);
  EngineRender.clear();

  levelManager.submitRender(EngineRender);
  if (m_portal) {
    EngineRender.submit(*m_portal);
#if DEBUG_DRAW_COLLISIONS
    sf::FloatRect phb = m_portal->getHitbox();
    sf::RectangleShape debugPortal({phb.width, phb.height});
    debugPortal.setPosition(phb.left, phb.top);
    debugPortal.setFillColor(sf::Color::Transparent);
    debugPortal.setOutlineColor(sf::Color::Magenta);
    debugPortal.setOutlineThickness(config::DEBUG_PORTAL_OUTLINE);
    EngineWindow->draw(debugPortal);
#endif
  }
  for (const auto &wp : m_weaponPickups)
    EngineRender.submit(*wp);
  EngineRender.submit(player);
  for (const auto &enemy : enemies)
    EngineRender.submit(*enemy);
  for (const auto &bullet : bullets)
    EngineRender.submit(*bullet);

  EngineRender.draw();
  EngineRender.display();
}

void Engine::update(const sf::Time &dt) {
  EngineInput.pollEvents(*EngineWindow, EngineEvent, EngineCamera);

  PlayerInputState state =
      EngineInput.getPlayerInput(*EngineWindow, player, resourceManager);

  player.setMousePos(state.mousePos);
  player.update(dt);
  player.moveShootTime(dt);

  if (m_portal)
    m_portal->update(dt);

  for (const auto &bullet : bullets)
    bullet->update(dt);
  for (const auto &enemy : enemies) {
    enemy->setTargetPosition(player.getPosition());
    enemy->update(dt);
  }

  for (auto &enemy : enemies) {
    sf::Vector2<float> oldPos = enemy->getPosition();
    enemy->move(dt, enemy->getVelocity());
    if (levelManager.checkCollision(*enemy)) {
      enemy->setPosition(oldPos);
    }
  }

  EnginePhysics.handleCollisions(bullets, enemies, levelManager);
  EnginePhysics.cleanup(bullets, enemies, levelManager);
  EnginePhysics.checkCollisions(dt, player, levelManager);

  player.handlePlayer(state, resourceManager, bullets);

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
    player.switchWeapon(0, resourceManager);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
    player.switchWeapon(1, resourceManager);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
    player.switchWeapon(2, resourceManager);

  std::optional<std::reference_wrapper<Room>> currentWrapper =
      levelManager.findRoomAt(player.getCenter());
  if (currentWrapper.has_value()) {
    Room &currentRoom = currentWrapper.value().get();
    bool isNewRoom = !m_previousRoom.has_value() ||
                     &m_previousRoom.value().get() != &currentRoom;

    if (isNewRoom && currentRoom.getRoomType() == RoomType::Combat)
      tryActivateCombat(currentRoom);

    m_previousRoom = std::ref(currentRoom);
  } else {
    m_previousRoom.reset();
  }

  if (combatManager.isSpawnPending()) {
    Room &activeRoom = combatManager.activeRoom();
    if (combatManager.tryCloseGates(activeRoom, player))
      combatManager.spawnWave(activeRoom, enemies, resourceManager);
  }

  tryCompleteCombatWave();

  if (!m_portal && roomsCleared(levelManager.getRooms()) &&
      m_portalRoom.has_value()) {
    spawnPortalInRoom(m_portalRoom.value().get());
  }

  if (state.wantToInteract) {
    std::unique_ptr<WeaponPickup> droppedPickup;
    for (auto it = m_weaponPickups.begin(); it != m_weaponPickups.end();) {
      if ((*it)->getHitbox().contains(state.mousePos)) {
        droppedPickup = swapWeapon(**it);
        it = m_weaponPickups.erase(it);
        break;
      }
      ++it;
    }
    if (droppedPickup)
      m_weaponPickups.push_back(std::move(droppedPickup));

    tryOpenChest(state.mousePos);
    tryAdvanceRound(state.mousePos);
  }

  EngineCamera.setCenter(player.getPosition());
}
