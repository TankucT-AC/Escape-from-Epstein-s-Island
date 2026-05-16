// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Engine.hpp"
#include "AuthClient.hpp"
#include "AuthService.hpp"
#include <iostream>
#include "config.hpp"

int main() {
    try {
        #if !DEBUG_NOT_AUTH
            AuthClient auth;
            AuthService authService(auth);
        if (!authService.run()) {
            return EXIT_SUCCESS;
        }
        #endif // DEBUG_NOT_AUTH
        Engine engine;
        engine.run();

    } catch (const std::exception& e) {
        std::cerr << "FATAL: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}