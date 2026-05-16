// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTH_SERVICE_HPP
#define AUTH_SERVICE_HPP

#include <SFML/Graphics.hpp>
#include <optional>
#include "AuthClient.hpp"
#include "UIComponents.hpp"

class AuthService {
private:
    sf::RenderWindow AuthWindow;
    sf::Event AuthEvent;
    sf::Font AuthFont;
    AuthClient& authClient;

    // Начинка окна
    ui::Button loginBtn;
    ui::Button regBtn;
    sf::Text loginLabel;
    sf::Text passLabel;
    sf::Text loginDisplay;
    sf::Text passDisplay;
    sf::Text status;
    
    std::string loginStr, passStr;
    bool isPasswordMode = false;

    static sf::String makeUtf8String(const char* source);
    void handleInput(sf::Event& event);
    void handleClicks(sf::Vector2i mousePos);
    static std::string getSystemFont();

    void render();
    std::optional<bool> update();

public:
    AuthService(AuthClient& auth);
    bool run();
};

#endif