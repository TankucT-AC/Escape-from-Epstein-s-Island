// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANIMATED_SPRITE_HPP
#define ANIMATED_SPRITE_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>

class AnimatedSprite {
private:
  int m_frameWidth = 0;
  int m_frameHeight = 0;
  int m_totalFrames = 1;
  int m_currentFrame = 0;
  float m_frameTime = 0.12f;
  float m_timer = 0.f;
  int m_idleRow = 0;
  int m_runRow = 0;

  sf::IntRect m_uvRect;

public:
  AnimatedSprite() = default;

  void init(int frameW, int frameH, int frameCount, float frameTime = 0.12f, int idleRow = 0, int runRow = 0) {
    m_frameWidth = frameW;
    m_frameHeight = frameH;
    m_totalFrames = frameCount;
    m_frameTime = frameTime;
    m_idleRow = idleRow;
    m_runRow = runRow;
    m_uvRect = sf::IntRect(0, idleRow * frameH, frameW, frameH);
  }

  void update(float dtSec, bool isMoving, sf::Sprite& sprite) {
    if (m_totalFrames <= 1)
      return;

    if (!isMoving) {
      m_currentFrame = 0;
      m_timer = 0.f;
      m_uvRect.left = 0;
      m_uvRect.top = m_idleRow * m_frameHeight;
      sprite.setTextureRect(m_uvRect);
      return;
    }

    m_timer += dtSec;
    if (m_timer >= m_frameTime) {
      m_timer -= m_frameTime;
      m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
      m_uvRect.left = m_currentFrame * m_frameWidth;
      m_uvRect.top = m_runRow * m_frameHeight;
      sprite.setTextureRect(m_uvRect);
    }
  }
};

#endif // ANIMATED_SPRITE_HPP
