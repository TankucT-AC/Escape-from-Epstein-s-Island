// Copyright (C) 2026 Magomed Gadzhiumarov
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "TOTPGenerator.h"
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <functional>

using namespace drogon;

class AuthController : public drogon::HttpController<AuthController> {
private:
  TOTPGenerator TOTP_gen;

public:
  METHOD_LIST_BEGIN
  ADD_METHOD_TO(AuthController::registerUser, "/api/register", Post);
  ADD_METHOD_TO(AuthController::register2FA, "/api/setup_2fa", Post);
  ADD_METHOD_TO(AuthController::loginUser, "/api/login", Post);
  ADD_METHOD_TO(AuthController::login2FA, "/api/verify_2fa", Post);
  METHOD_LIST_END

  void registerUser(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback);
  void register2FA(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback);
  void loginUser(const HttpRequestPtr &req,
                 std::function<void(const HttpResponsePtr &)> &&callback);
  void login2FA(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback);
};

#endif // AUTH_CONTROLLER_H