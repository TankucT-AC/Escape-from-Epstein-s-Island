cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(2DGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.6 REQUIRED COMPONENTS system window graphics audio network)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SFML)

set(SOURCES
    src/main.cpp
    src/Engine.cpp
    src/Player.cpp
    src/Bullet.cpp
)

set(HEADERS
    include/config.hpp
    include/Engine.hpp
    include/Player.hpp
    include/Bullet.hpp
    include/ResourceManager.hpp
    include/Entity.hpp
)

add_executable(game ${SOURCES} ${HEADERS})

target_include_directories(game PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    sfml-network
)