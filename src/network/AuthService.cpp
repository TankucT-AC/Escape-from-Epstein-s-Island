#include "src/network/AuthService.hpp"
#include <iostream>
#include <cctype> 

AuthService::AuthService(AuthClient& c) : client(c) {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    window.create(sf::VideoMode(450, 600), "Escape From Epstein Island", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setFramerateLimit(60);

    if (!font.loadFromFile(findSystemFont())) std::cerr << "Font error\n";
    setupUI();
}

void AuthService::setupUI() {
    titleText.setFont(font);
    titleText.setCharacterSize(24);
    titleText.setStyle(sf::Text::Bold);
    
    infoText.setFont(font);
    infoText.setCharacterSize(14);
    infoText.setFillColor(ui::ColorTextGray);

    statusText.setFont(font);
    statusText.setCharacterSize(13);
    
    generatedPassText.setFont(font);
    generatedPassText.setCharacterSize(28);
    generatedPassText.setFillColor(ui::ColorSuccess); 
    generatedPassText.setStyle(sf::Text::Bold);

    loginInput.init("USERNAME", {55, 160}, font);
    passInput.init("PASSWORD", {55, 240}, font, true);
    
    code2FAInput.init("6-DIGIT CODE", {125, 430}, font); 
    code2FAInput.box.setSize({200, 45});

    primaryBtn.init("CONTINUE", {55, 370}, {340, 45}, font);
    secondaryBtn.init("REGISTER", {55, 430}, {340, 45}, font, ui::ColorPanelBG, sf::Color(64, 68, 75));
    
    switchState(State::Login);
}

void AuthService::switchState(State s) {
    state = s;
    
    loginInput.isVisible = (s == State::Login || s == State::Register);
    passInput.isVisible = (s == State::Login);
    code2FAInput.isVisible = (s == State::Verify2FA || s == State::SetupQR);
    
    primaryBtn.isVisible = true;
    secondaryBtn.isVisible = (s == State::Login || s == State::Register || s == State::Ask2FA || s == State::Verify2FA);

    // Сброс позиций кнопок к дефолтным значениям (для Login и Register)
    primaryBtn.box.setPosition(55, 370);
    primaryBtn.centerText();
    secondaryBtn.box.setPosition(55, 430);
    secondaryBtn.centerText();

    if (s == State::Login) {
        titleText.setString("Welcome back!");
        infoText.setString("We're so excited to see you again!");
        primaryBtn.setLabel("LOGIN");
        secondaryBtn.setLabel("NEED AN ACCOUNT?");
    } else if (s == State::Register) {
        titleText.setString("Create an account");
        infoText.setString("Enter a username. We will generate a secure\npassword for you automatically.");
        primaryBtn.setLabel("REGISTER");
        secondaryBtn.setLabel("BACK TO LOGIN");
    } else if (s == State::Ask2FA) {
        titleText.setString("Account Created!");
        infoText.setString("SAVE THIS PASSWORD! You will not see it again.\n\nWould you like to enable 2FA protection?");
        primaryBtn.setLabel("YES, ENABLE 2FA");
        secondaryBtn.setLabel("SKIP & PLAY");
        
        generatedPassText.setString(generatedPassword);
        float infoBottom = infoText.getGlobalBounds().top + infoText.getGlobalBounds().height;
        generatedPassText.setPosition(225 - generatedPassText.getGlobalBounds().width/2.f, infoBottom + 20.f);
    } else if (s == State::SetupQR) {
        titleText.setString("Scan QR Code");
        infoText.setString("Open your Authenticator app and scan\nthe code below to finish setup.");
        primaryBtn.setLabel("FINISH SETUP");

        // Динамические координаты под QR-кодом
        float qrBottom = qrSprite.getGlobalBounds().top + qrSprite.getGlobalBounds().height;
        float code2FAY = qrBottom + 10.f;
        code2FAInput.box.setPosition(125.f, code2FAY);
        // Текст внутри поля: отступ 10px от левого края и 8px от верхнего края рамки
        code2FAInput.display.setPosition(code2FAInput.box.getPosition().x + 10.f,
                                         code2FAInput.box.getPosition().y + 8.f);
        
        float primaryBtnY = code2FAInput.box.getPosition().y + code2FAInput.box.getSize().y + 15.f;
        primaryBtn.box.setPosition(55.f, primaryBtnY);
        primaryBtn.centerText();
        
        secondaryBtn.isVisible = false;
        
        code2FAInput.value = ""; 
        code2FAInput.updateVisuals();
    } else if (s == State::Verify2FA) {
        titleText.setString("Enter Login Code");
        infoText.setString("Enter the 6-digit code from your app.");
        primaryBtn.setLabel("VERIFY");
        secondaryBtn.setLabel("BACK TO LOGIN");

        // Динамический Y после infoText
        float infoBottom = infoText.getGlobalBounds().top + infoText.getGlobalBounds().height;
        float code2FAY = infoBottom + 20.f;
        code2FAInput.box.setPosition(125.f, code2FAY);
        code2FAInput.display.setPosition(code2FAInput.box.getPosition().x + 10.f,
                                         code2FAInput.box.getPosition().y + 8.f);
        
        float primaryBtnY = code2FAInput.box.getPosition().y + code2FAInput.box.getSize().y + 15.f;
        primaryBtn.box.setPosition(55.f, primaryBtnY);
        primaryBtn.centerText();
        
        float secondaryBtnY = primaryBtnY + primaryBtn.box.getSize().y + 15.f;
        secondaryBtn.box.setPosition(55.f, secondaryBtnY);
        secondaryBtn.centerText();
        
        code2FAInput.value = ""; 
        code2FAInput.updateVisuals();
    }

    titleText.setPosition(225 - titleText.getGlobalBounds().width/2.f, 60);
    infoText.setPosition(225 - infoText.getGlobalBounds().width/2.f, 100);
}

void AuthService::handleEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) window.close();
        
        if (e.type == sf::Event::TextEntered) {
            loginInput.handleText(e.text.unicode);
            passInput.handleText(e.text.unicode);
            code2FAInput.handleText(e.text.unicode);
        }

        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i m = sf::Mouse::getPosition(window);
            
            loginInput.isFocused = loginInput.contains(m);
            passInput.isFocused = passInput.contains(m);
            code2FAInput.isFocused = code2FAInput.contains(m);
            loginInput.updateVisuals(); passInput.updateVisuals(); code2FAInput.updateVisuals();

            if (primaryBtn.contains(m)) {
                if (state == State::Login) {
                    currentLogin = loginInput.value;
                    auto res = client.login(currentLogin, passInput.value);
                    
                    if (res.success) {
                        if (res.requires_2fa) {
                           switchState(State::Verify2FA);
                           setStatus("2FA Token required.", ui::ColorSuccess);
                        } else {
                           authorized = true;
                        }
                    } else setStatus("Invalid credentials!", ui::ColorError);
                } 
                else if (state == State::Register) {
                    auto res = client.registerUser(loginInput.value);
                    if (res.success) {
                        currentLogin = loginInput.value;
                        generatedPassword = res.password; 
                        switchState(State::Ask2FA);
                        setStatus("Success!", ui::ColorSuccess);
                    } else setStatus(res.message, ui::ColorError);
                }
                else if (state == State::Ask2FA) {
                    auto res = client.setup2FA(currentLogin);
                    if (res.success && !res.qr_image_data.empty()) {
                        qrTexture.loadFromMemory(res.qr_image_data.data(), res.qr_image_data.size());
                        qrSprite.setTexture(qrTexture, true);
                       
                        float scale = 140.f / qrSprite.getLocalBounds().width;
                        qrSprite.setScale(scale, scale);
                        // Динамический Y: под infoText с отступом
                        float infoBottom = infoText.getGlobalBounds().top + infoText.getGlobalBounds().height;
                        qrSprite.setPosition(225 - qrSprite.getGlobalBounds().width/2.f, infoBottom + 20.f);
                        
                        switchState(State::SetupQR);
                    } else {
                        setStatus("Failed to generate QR", ui::ColorError);
                    }
                }
                else if (state == State::SetupQR || state == State::Verify2FA) {
                    // ФИКС ВВОДА: Очищаем строку от мусора, оставляем ТОЛЬКО цифры
                    std::string cleanDigits = "";
                    for (char c : code2FAInput.value) {
                        if (std::isdigit(c)) cleanDigits += c;
                    }

                    if (cleanDigits.empty()) {
                        setStatus("Enter a valid code!", ui::ColorError);
                    } else {
                        auto res = client.verify2FA(currentLogin, cleanDigits);
                        if (res.success) authorized = true;
                        else setStatus("Invalid 2FA code!", ui::ColorError);
                    }
                }
            }
            
            if (secondaryBtn.contains(m)) {
                if (state == State::Login) switchState(State::Register);
                else if (state == State::Register) switchState(State::Login); 
                else if (state == State::Ask2FA) authorized = true; 
                else if (state == State::Verify2FA) switchState(State::Login); // Возврат к логину из 2FA
            }
        }
    }
}

void AuthService::render() {
    window.clear(ui::ColorDarkBG);
    
    sf::RectangleShape p({400, 500}); p.setPosition(25, 40); p.setFillColor(ui::ColorPanelBG);
    window.draw(p);

    window.draw(titleText);
    window.draw(infoText);
    
    loginInput.draw(window);
    passInput.draw(window);
    code2FAInput.draw(window);
    
    if (state == State::Ask2FA) window.draw(generatedPassText);
    if (state == State::SetupQR) window.draw(qrSprite);

    primaryBtn.draw(window);
    secondaryBtn.draw(window);
    
    statusText.setPosition(225 - statusText.getGlobalBounds().width/2.f, 510);
    window.draw(statusText);
    
    window.display();
}

bool AuthService::run() {
    while (window.isOpen() && !authorized) {
        handleEvents();
        primaryBtn.update(sf::Mouse::getPosition(window));
        secondaryBtn.update(sf::Mouse::getPosition(window));
        render();
    }
    
    if (authorized && window.isOpen()) {
        window.close();
    }
    
    return authorized;
}

std::string AuthService::findSystemFont() {
    for (auto p : {"/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", "/usr/share/fonts/TTF/DejaVuSans.ttf"})
        if (FILE* f = fopen(p, "r")) { fclose(f); return p; }
    return "arial.ttf";
}

void AuthService::setStatus(const std::string& msg, sf::Color col) {
    statusText.setString(msg); statusText.setFillColor(col);
}
