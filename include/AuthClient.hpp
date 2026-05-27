// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTH_CLIENT_HPP
#define AUTH_CLIENT_HPP

#include "config.hpp"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <openssl/err.h>
#include <string>
#include <vector>

class AuthClient {
public:
    struct Response {
        bool success = false;
        bool requires_2fa = false;
        std::string message;
        std::string password;
        std::string totp_secret;
        std::vector<unsigned char> qr_image_data;
    };

    Response login(const std::string& user, const std::string& pass) {
        return request("/api/login", {{"login", user}, {"password", pass}});
    }

    Response registerUser(const std::string& user) {
        return request("/api/register", {{"login", user}});
    }

    Response setup2FA(const std::string& user) {
        auto res = request("/api/setup_2fa", {{"login", user}});
        if (res.success && !res.message.empty()) {
            // Предполагаем, что сервер возвращает Base64 в поле qr_code_base64
            res.qr_image_data = decodeBase64(res.message); 
        }
        return res;
    }

    Response verify2FA(const std::string& user, const std::string& code) {
        return request("/api/verify_2fa", {{"login", user}, {"totp_key", code}});
    }

private:
    static std::vector<unsigned char> decodeBase64(const std::string& in) 
    {
        std::vector<int> T(256, -1);
        for (int i = 0; i < 64; i++) 
            T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

        std::vector<unsigned char> out;
        int val = 0, valb = -8;
        for (unsigned char c : in) {
            if (c == '=') break; // Конец данных (padding)
            if (T[c] == -1) continue; // Игнорируем переносы строк и мусор
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0) {
                out.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        return out;
    }

    Response request(const std::string& path, const nlohmann::json& body) 
    {
        httplib::Client cli(config::SCHEME_HOST_PORT);
        cli.enable_server_certificate_verification(false);
        cli.set_connection_timeout(0, 500'000);
        cli.set_read_timeout(5, 0);

        Response out;

        if (auto res = cli.Post(path, body.dump(), "application/json")) {
            if (res->status == 200) {
                try {
                    auto data = nlohmann::json::parse(res->body);
                    std::string status = data.value("status", "error");
                    out.success = (status == "success");
                    out.requires_2fa = data.value("is_totp_verify", false);
                    
                    if (path == "/api/setup_2fa") {
                        out.message = data.value("qr_code_base64", ""); 
                        out.totp_secret = data.value("totp_secret", "");
                    } else {
                        out.message = status;
                    }
                    
                    out.password = data.value("password", "");
                } catch (const nlohmann::json::parse_error&) {
                    out.message = "JSON_ERROR";
                }
            } else {
                out.message = "SERVER_ERROR_" + std::to_string(res->status);
            }
        } else {
            unsigned long ssl_err = ERR_get_error();
            char err_buf[256] = {0};
            ERR_error_string_n(ssl_err, err_buf, sizeof(err_buf));
            out.message = "NET_ERR: " + std::string(err_buf);
        }

        return out;
    }
};

#endif