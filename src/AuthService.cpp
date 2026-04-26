#include "AuthService.hpp"
#include <optional>
#include <SFML/System/String.hpp>
#include "UIComponents.hpp"

AuthService::AuthService(AuthClient& auth) : authClient(auth),
    loginBtn("Login", {ui::Layout::LeftMargin, ui::Layout::ButtonAreaY}, AuthFont),
    regBtn("Register", {ui::Layout::LeftMargin + 150.f, ui::Layout::ButtonAreaY}, AuthFont),
    loginLabel("Login:", AuthFont, 18),
    passLabel("Password:", AuthFont, 18),
    loginDisplay("", AuthFont, 18),
    passDisplay("", AuthFont, 18),
    status(AuthService::makeUtf8String("Нажмите TAB для переключения полей"), AuthFont, 14)
{
    AuthWindow.create(sf::VideoMode(500, 400), "Authorization", sf::Style::Titlebar | sf::Style::Close);
    if (!AuthFont.loadFromFile(getSystemFont())) {
        throw std::runtime_error("Fonts not found!");
    }

    loginLabel.setPosition(ui::Layout::LeftMargin, ui::Layout::TopOffset);
    loginDisplay.setPosition(ui::Layout::LeftMargin + ui::Layout::LabelWidth, ui::Layout::TopOffset);

    float secondRowY = loginLabel.getPosition().y + ui::Layout::VerticalSpacing;
    passLabel.setPosition(ui::Layout::LeftMargin, secondRowY);
    passDisplay.setPosition(ui::Layout::LeftMargin + ui::Layout::LabelWidth, secondRowY);

    status.setPosition(ui::Layout::LeftMargin, ui::Layout::StatusAreaY);
    status.setFillColor(sf::Color(180, 180, 180));
}

std::string AuthService::getSystemFont()
{
    std::vector<std::string> paths = 
    {
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "assets/fonts/arial.ttf"
    };
    for (auto& p : paths) 
    {
        sf::Font f;
        if (f.loadFromFile(p)) return p;
    }
    return "";
}

void AuthService::handleInput(sf::Event& event) 
{
    // 1. Обработка системных клавиш (Tab)
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Tab) {
            isPasswordMode = !isPasswordMode;
        }
    }

    if (event.type == sf::Event::TextEntered) {
        // Игнорируем управляющие символы вроде Esc (27) или Tab (9), 
        // которые могут прийти в TextEntered
        if (event.text.unicode < 32 && event.text.unicode != 8) {
            return; 
        }

        if (event.text.unicode < 128) {
            char c = static_cast<char>(event.text.unicode);
            std::string& target = isPasswordMode ? passStr : loginStr;

            if (c == '\b') { // Backspace
                if (!target.empty()) target.pop_back();
            } else {
                target += c;
            }
        }
    }
}

bool AuthService::run() 
{
    while (AuthWindow.isOpen()) 
    {
        auto isAuth= this->update();
        if (isAuth.has_value() && isAuth.value()) return true;        

        this->render();        
    }
    return false;
}

std::optional<bool> AuthService::update()
{
    sf::Vector2i mPos = sf::Mouse::getPosition(AuthWindow);
    while (AuthWindow.pollEvent(AuthEvent)) 
    {
        if (AuthEvent.type == sf::Event::Closed) AuthWindow.close();
        handleInput(AuthEvent);

        // Если мы не нажали на кнопку левой мышью, то пропускаем
        if (!(AuthEvent.type == sf::Event::MouseButtonPressed && 
            AuthEvent.mouseButton.button == sf::Mouse::Left)) continue;
        
        
        if (loginBtn.isHovered(mPos)) 
        {
            status.setString("Connecting...");
            status.setFillColor(sf::Color::Yellow);
            auto res = authClient.login(loginStr, passStr);
            if (res.success) 
            {
                AuthWindow.close();
                return true;
            }
            status.setString("Error: " + res.message);
            status.setFillColor(sf::Color::Red);
        }

        if (regBtn.isHovered(mPos)) 
        {
            status.setString("Registering...");
            auto res = authClient.registerUser(loginStr);
            if (res.success) {
                passStr = res.password;
                isPasswordMode = true;
                status.setString("Registered! Password: " + res.password);
                status.setFillColor(sf::Color::Green);
            } else {
                status.setString("Error: " + res.message);
                status.setFillColor(sf::Color::Red);
            }
        }
    }

    loginBtn.update(mPos);
    regBtn.update(mPos);

    loginDisplay.setString(loginStr + (!isPasswordMode ? "_" : ""));
    passDisplay.setString(std::string(passStr.length(), '*') + (isPasswordMode ? "_" : ""));

    loginLabel.setFillColor(!isPasswordMode ? sf::Color::Cyan : sf::Color::White);
    passLabel.setFillColor(isPasswordMode ? sf::Color::Cyan : sf::Color::White);

    return std::nullopt;
}

void AuthService::render() 
{
    AuthWindow.clear(sf::Color(35, 35, 35));
    AuthWindow.draw(loginLabel);
    AuthWindow.draw(loginDisplay);
    AuthWindow.draw(passLabel);
    AuthWindow.draw(passDisplay);
    
    loginBtn.draw(AuthWindow);
    regBtn.draw(AuthWindow);
    
    AuthWindow.draw(status);
    AuthWindow.display();
}

sf::String AuthService::makeUtf8String(const char* source) 
{
    std::string s = source;
    return sf::String::fromUtf8(s.begin(), s.end());
}