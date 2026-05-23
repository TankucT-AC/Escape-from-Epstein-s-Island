// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#include "AuthController.h"
#include "DatabaseManager.h"
#include "TOTPGenerator.h"
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <json/value.h>
#include <sqlite3.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <memory>
#include <optional>
#include <chrono>

bool verifyPassword(const std::string& password, const std::string& storedHash) 
{
    size_t colonPos = storedHash.find(':');
    if (colonPos == std::string::npos) return false;

    std::string saltHex = storedHash.substr(0, colonPos);
    std::string originalHashHex = storedHash.substr(colonPos + 1);

    std::vector<unsigned char> salt;
    for (size_t i = 0; i < saltHex.length(); i += 2) {
        salt.push_back(std::stoi(saltHex.substr(i, 2), nullptr, 16));
    }

    const int KEYLEN = 32;
    std::vector<unsigned char> newHash(KEYLEN);
    PKCS5_PBKDF2_HMAC(
        password.c_str(), 
        password.size(), 
        salt.data(), 
        salt.size(), 
        600000, 
        EVP_sha256(), 
        KEYLEN, 
    newHash.data());

    std::stringstream ss;
    for (unsigned char b : newHash) ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    
    return ss.str() == originalHashHex;
}

void AuthController::registerUser(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) 
{
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    Json::Value res;

    if (!json || !(*json).isMember("login")) 
    {
        res["status"] = "error";
        res["message"] = "Login required";
        callback(HttpResponse::newHttpJsonResponse(res));
        return;
    }

    std::string login = (*json)["login"].asString();
    std::string password = TOTP_gen.generateRandomPass();
    std::string password_hash = TOTP_gen.passHash(password);

    if (DatabaseManager::instance().insertNewUser(login, password_hash)) 
    { 
        res["status"] = "success";
        res["password"] = password; // отправляю чистый пароль юзеру
    } else {
        res["status"] = "error";
        res["message"] = "User already exists";
    }
    
    callback(HttpResponse::newHttpJsonResponse(res));
}

void AuthController::loginUser(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) 
{
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    Json::Value res;

    Json::Value& val = *json;
    std::string login = val["login"].asString();
    std::string password = val["password"].asString();

    bool success = false;
    std::optional<std::string> db_hash = DatabaseManager::instance().findHashPass(login);
    if (db_hash.has_value()) 
    {
        success = verifyPassword(password, db_hash.value());
    }
    
    res["status"] = success ? "success" : "fail";
    
    callback(HttpResponse::newHttpJsonResponse(res));
}

void AuthController::register2FA(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    Json::Value res;

    std::string login = (*json)["login"].asString();
    
    std::optional<std::string> TOTP_2FA = DatabaseManager::instance().connect2FA(login);
    
    res["status"] = TOTP_2FA.has_value() ? "success" : "fail";
    if (TOTP_2FA.has_value()) res["totp_secret"] = TOTP_2FA.value();
    
    callback(HttpResponse::newHttpJsonResponse(res));
}

void AuthController::login2FA(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    Json::Value res;

    std::string login = (*json)["login"].asString();
    std::string TOTP_key = (*json)["totp_key"].asString();
    
    std::optional<std::string> TOTP_secret = DatabaseManager::instance().getTOTPSecret(login);
    if (TOTP_secret.has_value())
    {
        auto now = std::chrono::system_clock::now();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

        bool is_success = false;
        for (int offset = -30; offset <= 30; offset += 30)
        {
            unsigned long long currTime = seconds.count() + offset;
            std::string currHexUnixTime = TOTP_gen.getHexUnixTime(currTime / 30);
            std::string serverTOTPkey = TOTP_gen.generate(TOTP_secret.value(), currHexUnixTime, "6", "HmacSHA1");
            
            if (serverTOTPkey.length() == TOTP_key.length() &&
                    CRYPTO_memcmp(serverTOTPkey.c_str(), TOTP_key.c_str(), serverTOTPkey.length()) == 0)
            {
                is_success = true;
            }
        }

        res["status"] = (is_success) ? "success" : "fail";
    } else {
        res["status"] = "error";
    }

    callback(HttpResponse::newHttpJsonResponse(res));
}