#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <memory>
#include <vector>
#include "Player.hpp"
#include "Bullet.hpp"
#include "ResourceManager.hpp"

class Engine
{
private:
    std::unique_ptr<sf::RenderWindow> EngineWindow;
    sf::VideoMode EngineVideoMode;
    sf::Event EngineEvent;
    sf::Clock EngineClock;
    sf::View EngineCamera;
    ResourceManager resourceManager;
    Player player;
    std::vector<std::unique_ptr<Bullet>> bullets;

    // КОСТЫЛЬ ДЛЯ ТЕСТА
    sf::RectangleShape board;

public:
    Engine();

    void run();

    void render();

    void update(const sf::Time& dt);
};

#endif