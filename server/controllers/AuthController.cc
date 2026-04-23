#include "AuthController.h"
#include "DatabaseManager.h"
#include <sqlite3.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <random>

std::string passHash(const std::string& password) 
{
    const int SALT_SIZE = 16;
    std::vector<unsigned char> salt(SALT_SIZE); 
    if (RAND_bytes(salt.data(), SALT_SIZE) != 1) 
    {
        throw std::runtime_error("RAND_bytes failed");
    }

    const int KEYLEN = 32;
    std::vector<unsigned char> hash(KEYLEN);

    if (PKCS5_PBKDF2_HMAC(
            password.c_str(), password.size(), 
            salt.data(), salt.size(), 
            600000,           // итерации
            EVP_sha256(),   // алгоритм хеширования
            KEYLEN,         // желаемая длина выхода
            hash.data()        // куда записать
        ) != 1) {
        throw std::runtime_error("PBKDF2 failed");
    }

    std::stringstream ss;
    for (unsigned char b : salt) ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    ss << ":";
    for (unsigned char b : hash) ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;

    return ss.str();
}

std::string generateRandomPass() noexcept
{
    static const std::string alphabet = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> randSymbol(0, alphabet.size() - 1);

    std::string result;
    const int PASS_LEN = 10;
    for (int i = 0; i < PASS_LEN; ++i) 
    {
        auto new_char = alphabet[randSymbol(gen)];
        result.push_back(new_char);
    }
    
    return result; 
}

bool verifyPassword(const std::string& password, const std::string& storedHash) {
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

// TODO: вынести магические числа и названия в отдельный файл или в приватные поля класса
void AuthController::registerUser(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) 
{
    auto json = req->getJsonObject();
    Json::Value res;

    if (!json || !(*json).isMember("login")) 
    {
        res["status"] = "error";
        res["message"] = "Login required";
        callback(HttpResponse::newHttpJsonResponse(res));
        return;
    }

    std::string login = (*json)["login"].asString();
    std::string password = generateRandomPass();
    std::string password_hash = passHash(password);

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
    auto json = req->getJsonObject();
    Json::Value res;

    auto& val = *json;
    std::string login = val["login"].asString();
    std::string password = val["password"].asString();

    bool success = false;
    auto db_hash = DatabaseManager::instance().findHashPass(login);
    if (db_hash.has_value()) 
    {
        success = verifyPassword(password, db_hash.value());
    }
    
    res["status"] = success ? "success" : "fail";
    
    callback(HttpResponse::newHttpJsonResponse(res));
}