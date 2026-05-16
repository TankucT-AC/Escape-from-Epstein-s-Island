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

class AuthClient {
public:
    struct Response {
        bool success = false;
        std::string message;
        std::string password;
    };

    Response login(const std::string& user, const std::string& pass) {
        return request("/api/login", {{"login", user}, {"password", pass}});
    }

    Response registerUser(const std::string& user) {
        return request("/api/register", {{"login", user}});
    }

private:
    Response request(const std::string& path, const nlohmann::json& body) 
    {
        httplib::Client cli(config::SCHEME_HOST_PORT);
        cli.enable_server_certificate_verification(false);

        cli.set_connection_timeout(0, 500'000);
        cli.set_read_timeout(5, 0);            // 5 сек

        Response out;

        if (auto res = cli.Post(path, body.dump(), "application/json")) {
            if (res->status == 200) {
                try {
                    auto data = nlohmann::json::parse(res->body);
                    out.message = data.value("status", "error");
                    out.success = (out.message == "success");
                    out.password = data.value("password", "");
                } catch (const nlohmann::json::parse_error&) {
                    out.message = "JSON_ERROR";
                }
            } else {
                out.message = "SERVER_ERROR_" + std::to_string(res->status);
            }
        } else {
            httplib::Error err = res.error();
            unsigned long ssl_err = ERR_get_error();
            char err_buf[256];
            ERR_error_string_n(ssl_err, err_buf, sizeof(err_buf));
            
            out.message = "SSL_ERR: " + std::string(err_buf);
        }

        return out;
    }
};

#endif