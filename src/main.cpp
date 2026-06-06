// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "src/core/Engine.hpp"
#include "src/network/AuthClient.hpp"
#include "src/network/AuthService.hpp"
#include <iostream>
#include "src/core/config.hpp"

int main() {
    try {
        #if !DEBUG_NOT_AUTH
            AuthClient auth;
            AuthService authService(auth); 
            
            if (!authService.run()) {
                std::cout << "[Main] Авторизация не удалась или окно закрыто. Выход.\n";
                return EXIT_SUCCESS;
            }
        #endif // DEBUG_NOT_AUTH

        // Если дошли сюда, значит авторизация успешна (или дебаг)
        Engine engine;
        engine.run();

    } catch (const std::exception& e) {
        std::cerr << "FATAL: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}