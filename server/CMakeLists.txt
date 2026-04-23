cmake_minimum_required(VERSION 3.10)
project(server CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ищем только Drogon. Он сам подтянет JsonCpp, UUID и SQLite3
find_package(Drogon CONFIG REQUIRED)

# Список всех исходных файлов
set(SOURCE_FILES
    main.cc
    controllers/AuthController.cc
    controllers/DatabaseManager.cc
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Подключаем папки с хедерами
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/controllers
    ${CMAKE_CURRENT_SOURCE_DIR}/models
)

# Линкуем только Drogon. 
# Так как Drogon зависит от SQLite3, она линкуется к твоему проекту автоматически.
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Drogon::Drogon
)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views ${CMAKE_CURRENT_BINARY_DIR})