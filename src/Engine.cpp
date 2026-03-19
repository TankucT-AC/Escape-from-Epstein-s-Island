#include "Engine.hpp"
#include "Bullet.hpp"
#include "config.hpp"
#include <cmath>
#include <memory>

Engine::Engine() : 
player(resourceManager.getTexture(config::PLAYER_TEXTURE))
{
    EngineVideoMode.height = config::GAMEBOARD_HEIGHT;
    EngineVideoMode.width  = config::GAMEBOARD_WIDTH;
    EngineWindow = std::make_unique<sf::RenderWindow>(EngineVideoMode, config::GAMEBOARD_NAME);
    EngineCamera.setCenter(player.getPosition());

    enemies.push_back(std::make_unique<Enemy>(resourceManager.getTexture(config::ENEMY_TEXTURE)));

    // КОСТЫЛЬ
    board.setSize(sf::Vector2<float>{500.f, 500.f});
    board.setFillColor(sf::Color::Cyan);
}

void Engine::run()
{
    while (EngineWindow->isOpen())
    {
        auto dt = EngineClock.restart();
        this->update(dt);
        this->render();
    }
}

void Engine::render()
{
    EngineWindow->setView(EngineCamera);
    EngineWindow->clear();
    
    // draw game loop
    EngineWindow->draw(board);
    player.draw(*EngineWindow);
    for (const auto& enemy : enemies)
    {
        enemy->draw(*EngineWindow);
    }
        
    for (const auto& bullet : bullets)
    {
        bullet->draw(*EngineWindow);
    }

    EngineWindow->display();
}

void Engine::update(const sf::Time& dt)
{
    while (EngineWindow->pollEvent(EngineEvent))
    {
        if (EngineEvent.type == sf::Event::Closed)
            EngineWindow->close();
    }
    
    player.update(dt, *EngineWindow);
    player.moveShootTime(dt);
    for (const auto& bullet : bullets)
    {
        bullet->update(dt, *EngineWindow);
    }

    // Удаляем пули, срок жизни которых закончился
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), 
        [](const auto& bullet) 
        {   
            return !bullet->isBulletAlive(); 
        }), 
        bullets.end());
    
    // Наносим урон противнику
    for (const auto& bullet : bullets)
    {
        if (!bullet->isBulletAlive()) continue;
        for (const auto& enemy : enemies)
        {
            if (enemy->isBulletCollision(*bullet)) 
            {
                enemy->getReceivedDamage(bullet->getDamage());
                bullet->setBulletAlive(false);
            }
        }
        
    }

     enemies.erase(std::remove_if(enemies.begin(), enemies.end(), 
        [](const auto& enemy) 
        {   
            return !enemy->isEnemyAlive(); 
        }), 
        enemies.end());
    
    for (const auto& enemy : enemies)
    {
        enemy->update(dt, player.getPosition(), *EngineWindow);
    }
    
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && player.isShootTime()) 
    {
        sf::Vector2<float> mousePos = EngineWindow->mapPixelToCoords(
            sf::Mouse::getPosition(*EngineWindow));
        sf::Vector2<float> dir = mousePos - player.getPosition();

        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0) dir /= len;

        bullets.push_back(std::make_unique<Bullet>(
            resourceManager.getTexture(config::BULLET_PLAYER_TEXTURE),
            player.getPosition(),
            dir)
        );

        player.cooldown();
    }

    EngineCamera.setCenter(player.getPosition());
}