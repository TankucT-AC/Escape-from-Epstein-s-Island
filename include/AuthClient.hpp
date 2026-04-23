#ifndef AUTH_CLIENT_HPP
#define AUTH_CLIENT_HPP

#include "config.hpp"
#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>

class AuthClient {
public:
#if !DEBUG_AUTH
    bool devMode = false;
#else
    bool devMode = true; 
#endif

    struct Response {
        bool success = false;
        std::string message;
        std::string password;
    };

    Response login(const std::string& user, const std::string& pass) 
    {
        if (devMode) return {true, "success"};
        return request("/api/login", {{"login", user}, {"password", pass}});
    }

    Response registerUser(const std::string& user) 
    {
        if (devMode) return {true, "success", "dev_pass"};
        return request("/api/register", {{"login", user}});
    }

private:
    Response request(const std::string& path, const nlohmann::json& body) 
    {
        sf::Http http("http://127.0.0.1", 8080);
        sf::Http::Request req(path, sf::Http::Request::Post);
        req.setField("Content-Type", "application/json");
        req.setBody(body.dump());

        auto res = http.sendRequest(req);
        Response out;

        if (res.getStatus() == sf::Http::Response::Ok) 
        {
            try {
                auto data = nlohmann::json::parse(res.getBody());
                out.message = data.value("status", "error");
                out.success = (out.message == "success");
                out.password = data.value("password", "");
            } catch (...) { out.message = "JSON_ERROR"; }
        } else {
            out.message = "SERVER_OFFLINE";
        }
        return out;
    }
};

#endif