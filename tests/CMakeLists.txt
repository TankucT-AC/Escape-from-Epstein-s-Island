include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

find_package(OpenSSL REQUIRED)
find_package(SFML 2.6 REQUIRED COMPONENTS system window graphics audio network)

add_executable(test_auth
    test_auth.cpp
)

target_include_directories(test_auth PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)
target_link_libraries(test_auth PRIVATE
    game_core
    gtest
    gtest_main
    gmock
    gmock_main
    sfml-graphics sfml-window sfml-system sfml-audio sfml-network
    OpenSSL::SSL OpenSSL::Crypto
    pthread
)
include(GoogleTest)
gtest_discover_tests(test_auth)