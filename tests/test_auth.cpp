#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "src/network/AuthClient.hpp"
#include "third-party/cpp-httplib/httplib.h"

using ::testing::_;
using ::testing::Return;
using ::testing::ByMove;

class MockClient : public IHttpClient {
public:
    MOCK_METHOD(httplib::Result, Post,
                (const std::string&, const std::string&, const std::string&),
                (override));
};

class AuthClientTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_client_ = std::make_unique<MockClient>();
    }
    std::unique_ptr<MockClient> mock_client_;
};

// Вспомогательная функция, которая собирает httplib::Result
static httplib::Result make_response(int status, const std::string& body) {
    auto resp = std::make_unique<httplib::Response>();
    resp->status = status;
    resp->body = body;
    // Возвращаем Result. Третий аргумент - пустые заголовки
    return httplib::Result(std::move(resp), httplib::Error::Success, {});
}

// Вспомогательная функция для имитации сетевой ошибки (возвращаем nullptr внутри Result)
static httplib::Result make_network_error() {
    return httplib::Result(nullptr, httplib::Error::Connection, {});
}


TEST_F(AuthClientTest, LoginSuccess) {
    std::string json = R"({"status":"success","password":"secret"})";
    
    EXPECT_CALL(*mock_client_, Post("/api/login", _, "application/json"))
        .WillOnce(Return(ByMove(make_response(200, json))));

    AuthClient auth(std::move(mock_client_));
    auto res = auth.login("user", "pass");
    EXPECT_TRUE(res.success);
    EXPECT_FALSE(res.requires_2fa);
    EXPECT_EQ(res.password, "secret");
}

// Тест сетевой ошибки
TEST_F(AuthClientTest, LoginNetworkError) {
    EXPECT_CALL(*mock_client_, Post(_, _, _))
        .WillOnce(Return(ByMove(make_network_error()))); // Имитация ошибки сети

    AuthClient auth(std::move(mock_client_));
    auto res = auth.login("user", "pass");
    EXPECT_FALSE(res.success);
    EXPECT_THAT(res.message, ::testing::StartsWith("NET_ERR:"));
}

// Тест регистрации
TEST_F(AuthClientTest, RegisterSuccess) {
    std::string json = R"({"status":"success"})";
    EXPECT_CALL(*mock_client_, Post("/api/register", _, _))
        .WillOnce(Return(ByMove(make_response(200, json))));

    AuthClient auth(std::move(mock_client_));
    auto res = auth.registerUser("newuser");
    EXPECT_TRUE(res.success);
}