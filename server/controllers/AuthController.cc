// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#define STB_IMAGE_WRITE_IMPLEMENTATION
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

#include "qrcodegen.hpp"
#include "stb_image_write.h"

std::string generateQrCode(const std::string& uri)
{
    qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(uri.c_str(), qrcodegen::QrCode::Ecc::LOW);

    int size = qr.getSize();
    int scale = 4;  // Каждый квадратик QR будет 4x4 пикселя
    int border = 2; // Белая рамка вокруг (требование стандарта QR)
    int imgSize = (size + border * 2) * scale;

    // Создаем буфер для RGB (3 байта на пиксель), заливаем белым цветом (255)
    std::vector<unsigned char> pixels(imgSize * imgSize * 3, 255);

    // Закрашиваем черные модули
    for (int y = 0; y < size; y++) 
    {
        for (int x = 0; x < size; x++) 
        {
            if (qr.getModule(x, y)) 
            {
                // Если модуль черный, закрашиваем квадрат scale x scale
                int startX = (x + border) * scale;
                int startY = (y + border) * scale;
                for (int dy = 0; dy < scale; dy++) 
                {
                    for (int dx = 0; dx < scale; dx++) 
                    {
                        int pixelIndex = ((startY + dy) * imgSize + (startX + dx)) * 3;
                        pixels[pixelIndex] = 0;     // R
                        pixels[pixelIndex + 1] = 0; // G
                        pixels[pixelIndex + 2] = 0; // B
                    }
                }
            }
        }
    }

    int pngLength = 0;
    // stbi_write_png_to_mem возвращает сырой указатель на буфер с PNG-данными
    unsigned char* pngData = stbi_write_png_to_mem(
        pixels.data(), 
        0, // stride (автоматически)
        imgSize, 
        imgSize, 
        3, // компонентов на пиксель (RGB)
        &pngLength
    );

    std::string pngString(reinterpret_cast<const char*>(pngData), pngLength);
    STBIW_FREE(pngData); // Очищаем память, выделенную stb

    return pngString;
}

bool verifyPassword(const std::string& password, const std::string& storedHash) 
{
    size_t colonPos = storedHash.find(':');
    if (colonPos == std::string::npos) return false;

    std::string saltHex = storedHash.substr(0, colonPos);
    std::string originalHashHex = storedHash.substr(colonPos + 1);

    std::vector<unsigned char> salt;
    for (size_t i = 0; i < saltHex.length(); i += 2) 
    {
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

    std::optional<std::string> TOTP_secret = DatabaseManager::instance().getTOTPSecret(login);
    res["is_totp_verify"] = TOTP_secret.has_value();
    
    callback(HttpResponse::newHttpJsonResponse(res));
}

void AuthController::register2FA(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    Json::Value res;

    std::string login = (*json)["login"].asString();
    
    std::optional<std::string> TOTP_secret = DatabaseManager::instance().connect2FA(login);
    
    if (!TOTP_secret.has_value())
    {
        res["status"] = "fail";
    } else {
        res["status"] = TOTP_secret.has_value() ? "success" : "fail";

        std::string app_name = "EscapeFromEpsteinIsland";
        std::string uri = "otpauth://totp/" + app_name + ":" + login + 
                  "?secret=" + TOTP_secret.value() + 
                  "&issuer=" + app_name;
        
        std::string pngString = generateQrCode(uri);
        res["qr_code_base64"] = drogon::utils::base64Encode(
            reinterpret_cast<const unsigned char*>(pngString.data()), 
            pngString.size()
        );

        res["totp_secret"] = TOTP_secret.value();
    }
    
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