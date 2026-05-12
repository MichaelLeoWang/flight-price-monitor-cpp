#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    std::string database_path = "./flight_monitor.db";
    std::string api_key;
    std::string api_secret;
    std::string smtp_server = "smtp.gmail.com";
    int smtp_port = 587;
    std::string smtp_user;
    std::string smtp_password;
    int port = 8000;
};

bool loadConfig(const std::string& path, Config& config);

#endif