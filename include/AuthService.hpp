#ifndef AUTH_SERVICE_HPP
#define AUTH_SERVICE_HPP

#include <SFML/Graphics.hpp>
#include "AuthClient.hpp"
#include "UIComponents.hpp"

class AuthService {
public:
    enum class State { Login, Register, Ask2FA, SetupQR, Verify2FA };

    AuthService(AuthClient& client);
    bool run(); // Главный цикл окна

private:
    sf::RenderWindow window;
    sf::Font font;
    AuthClient& client;
    State state = State::Login;
    bool authorized = false;

    // UI элементы
    ui::Input loginInput, passInput, code2FAInput;
    ui::Button primaryBtn, secondaryBtn;
    sf::Text statusText, titleText, infoText;
    
    sf::Texture qrTexture;
    sf::Sprite qrSprite;
    std::string currentLogin;
    sf::Text generatedPassText;
    std::string generatedPassword;

    void setupUI();
    void update(sf::Vector2i mPos);
    void render();
    void handleEvents();
    void switchState(State newState);
    void setStatus(const std::string& msg, sf::Color col = sf::Color::White);
    
    // Системный шрифт
    std::string findSystemFont();
};

#endif