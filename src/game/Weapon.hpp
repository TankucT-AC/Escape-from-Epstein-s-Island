// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "Bullet.hpp"
#include "src/core/ResourceManager.hpp"
#include "src/core/config.hpp"
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

class Weapon {
protected:
  std::string m_weaponTexPath;
  std::string m_bulletTexPath;
  float m_fireRate;
  float m_damage;
  float m_bulletSpeed;
  float m_scale;

public:
  Weapon(const std::string &weaponTex, const std::string &bulletTex,
         float fireRate, float damage, float bulletSpeed, float scale = 1.f)
      : m_weaponTexPath(weaponTex), m_bulletTexPath(bulletTex),
        m_fireRate(fireRate), m_damage(damage), m_bulletSpeed(bulletSpeed),
        m_scale(scale) {}

  virtual ~Weapon() = default;

  virtual void fire(sf::Vector2<float> pos, sf::Vector2<float> dir,
                    std::vector<std::unique_ptr<Bullet>> &bullets,
                    ResourceManager &rm) = 0;

  float getFireRate() const { return m_fireRate; }
  float getDamage() const { return m_damage; }
  float getWeaponScale() const { return m_scale; }
  const std::string &getWeaponTexPath() const { return m_weaponTexPath; }
  const std::string &getBulletTexPath() const { return m_bulletTexPath; }
};

class RangedWeapon : public Weapon {
public:
  using Weapon::Weapon;

  void fire(sf::Vector2<float> pos, sf::Vector2<float> dir,
            std::vector<std::unique_ptr<Bullet>> &bullets,
            ResourceManager &rm) override {
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len > 0)
      dir /= len;
    auto radians = std::atan2(dir.y, dir.x);
    auto degrees = radians * 180.f / config::PI;
    bullets.push_back(std::make_unique<Bullet>(
        rm.getTexture(m_bulletTexPath), pos, dir, degrees, m_bulletSpeed,
        config::BULLET_DEFAULT_LIFETIME, m_damage));
  }
};

class MeleeWeapon : public Weapon {
public:
  MeleeWeapon(const std::string &weaponTex, float damage)
      : Weapon(weaponTex, "", config::MELEE_FIRE_RATE, damage, 0.f) {}

  void fire(sf::Vector2<float> pos, sf::Vector2<float> dir,
            std::vector<std::unique_ptr<Bullet>> &bullets,
            ResourceManager &rm) override {}
};

#endif