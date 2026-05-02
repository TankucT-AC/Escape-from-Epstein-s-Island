cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(2DGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

find_package(SFML 2.6 REQUIRED COMPONENTS system window graphics audio network)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SFML)

set(SOURCES
    src/main.cpp
    src/Engine.cpp
    src/Player.cpp
    src/Bullet.cpp
    src/Enemy.cpp
    src/Room.cpp
    src/Wall.cpp
    src/AuthService.cpp
)

set(HEADERS
    include/config.hpp
    include/Engine.hpp
    include/Player.hpp
    include/Bullet.hpp
    include/ResourceManager.hpp
    include/Entity.hpp
    include/Enemy.hpp
    include/Room.hpp
    include/UpdateContext.hpp
    include/Wall.hpp
    include/AuthClient.hpp
    include/AuthService.hpp
    include/UIComponents.hpp
    include/httplib.h
)

add_executable(game ${SOURCES} ${HEADERS})

target_include_directories(game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(game PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    sfml-network
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
)