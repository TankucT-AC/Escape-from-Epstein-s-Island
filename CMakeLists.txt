cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(2DGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

find_package(SFML 2.6 REQUIRED COMPONENTS system window graphics audio network)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SFML)

add_library(game_core STATIC
    src/core/config.hpp
)

# Core
target_sources(game_core PRIVATE
    src/core/Engine.hpp
    src/core/Engine.cpp
    src/core/ResourceManager.hpp
    src/core/InputManager.hpp
    src/core/InputManager.cpp
    src/core/RenderManager.hpp
    src/core/RenderManager.cpp
    src/core/PhysicsManager.hpp
    src/core/PhysicsManager.cpp
)

# Game
target_sources(game_core PRIVATE
    src/game/Bullet.hpp
    src/game/Bullet.cpp
    src/game/Enemy.hpp
    src/game/Enemy.cpp
    src/game/Entity.hpp
    src/game/Player.hpp
    src/game/Player.cpp
)

# Network
target_sources(game_core PRIVATE
    src/network/AuthClient.hpp
    src/network/AuthService.hpp
    src/network/AuthService.cpp
)

# UI
target_sources(game_core PRIVATE
    src/UI/UIComponents.hpp
)


# World
target_sources(game_core PRIVATE
    src/world/DungeonGenerator.hpp
    src/world/DungeonGenerator.cpp
    src/world/Prefabs.hpp
    src/world/Room.hpp
    src/world/Room.cpp
    src/world/Wall.hpp
    src/world/Wall.cpp
    src/world/Floor.hpp
    src/world/Floor.cpp
)

# third-party
target_sources(game_core PRIVATE
    third-party/cpp-httplib/httplib.h
)

target_include_directories(game_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(game_core PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    sfml-network
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
)

add_executable(game src/main.cpp)
target_link_libraries(game PRIVATE game_core)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# copy assets in bin
add_custom_command(TARGET game_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:game_core>/assets"
)
