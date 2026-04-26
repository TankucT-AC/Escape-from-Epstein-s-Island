#include "Engine.hpp"
#include "Bullet.hpp"
#include "UpdateContext.hpp"
#include "config.hpp"
#include <cmath>
#include <memory>

Engine::Engine() : 
player(resourceManager.getTexture(config::PLAYER_TEXTURE), sf::Vector2<float>{config::TILE_SIZE * 2, config::TILE_SIZE * 2})
{
    EngineVideoMode.height = config::GAMEBOARD_HEIGHT;
    EngineVideoMode.width  = config::GAMEBOARD_WIDTH;
    EngineWindow = std::make_unique<sf::RenderWindow>(EngineVideoMode, config::GAMEBOARD_NAME);
    EngineCamera.setCenter(player.getPosition());

    enemies.push_back(std::make_unique<Enemy>(resourceManager.getTexture(config::ENEMY_TEXTURE), sf::Vector2<float>{0.f, 0.f}));

    roomBlueprints = 
    {
        MapData::SQUARE_ROOM
    };

    room = std::make_unique<Room>(roomBlueprints[0], 
        sf::Vector2<float>(0.f, 0.f), resourceManager);

    // TODO: костыль, потом уберу
    test_wall = std::make_unique<Wall>(resourceManager.getTexture(config::DEFAULT_WALL_TEXTURE), 
        sf::Vector2<float>{config::TILE_SIZE * 2, config::TILE_SIZE * 2});
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

    // TODO: реализовать уровни 
    room->draw(*EngineWindow);
    // TODO: костыль, потом уберу
    test_wall->draw(*EngineWindow);
    player.draw(*EngineWindow);
    for (const auto& enemy : enemies)
    {
        //enemy->draw(*EngineWindow);
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

    player.update(UpdateContext(dt, *EngineWindow, *room));
    player.moveShootTime(dt);
    for (const auto& bullet : bullets)
    {
        bullet->update(UpdateContext(dt, *EngineWindow, *room));
    }

    // Удаляем пули, срок жизни которых закончился
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(), 
        [](const auto& bullet) 
        {   
            return !bullet->isBulletAlive(); 
        }), 
        bullets.end());
    
    
    for (const auto& bullet : bullets)
    {
        if (!bullet->isBulletAlive()) continue;
        // Наносим урон противнику
        for (const auto& enemy : enemies)
        {
            if (enemy->isBulletCollision(*bullet)) 
            {
                enemy->getReceivedDamage(bullet->getDamage());
                bullet->setBulletAlive(false);
            }
        }

        // Проверка пули на то что она столкнулась со стеной
        if (room->checkCollision(*bullet)) bullet->setBulletAlive(false);
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
        // Задаем вектор для пули
        sf::Vector2<float> mousePos = EngineWindow->mapPixelToCoords(
            sf::Mouse::getPosition(*EngineWindow));
        sf::Vector2<float> dir = mousePos - player.getPosition();
        
        // Нормализуем вектор
        float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0) dir /= len;

        auto radians = static_cast<float>(std::atan2(dir.y, dir.x));

        auto degrees = radians * 180.f / config::PI;
        
        // Добавляем пулю в массив
        bullets.push_back(std::make_unique<Bullet>(
            resourceManager.getTexture(config::BULLET_PLAYER_TEXTURE),
            player.getPosition(),
            dir,
            degrees)
        );

        // Вешаем КД на игрока
        player.cooldown();
    }

    // Центрируем камеру относительно игрока
    EngineCamera.setCenter(player.getPosition());
}