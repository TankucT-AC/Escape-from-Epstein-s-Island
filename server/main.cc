#include <drogon/drogon.h>
#include "DatabaseManager.h"

int main() {
    try {
        DatabaseManager::instance();
        LOG_INFO << "Database system online.";
    } catch (const std::exception& e) {
        LOG_ERROR << "Database failed to start: " << e.what();
        return 1;
    }

    drogon::app().loadConfigFile("../config.json").run();
    return 0;
}