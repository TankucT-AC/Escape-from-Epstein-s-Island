#include "Engine.hpp"
#include "AuthClient.hpp"
#include "AuthService.hpp"
#include <iostream>

int main() {
    try {
        AuthClient auth;
        AuthService authService(auth);

        if (!authService.run()) {
            return EXIT_SUCCESS;
        }

        Engine engine;
        engine.run();

    } catch (const std::exception& e) {
        std::cerr << "FATAL: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}