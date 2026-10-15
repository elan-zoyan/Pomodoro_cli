#pragma once

#include "pomodoro/session_manager.hpp"

#include <filesystem>

namespace pomodoro {

struct AppConfig {
    SessionDurations durations{};
    bool sound_enabled = true;
};

class ConfigManager {
public:
    explicit ConfigManager(std::filesystem::path config_path);

    AppConfig load() const;
    void save(const AppConfig& config) const;

    const std::filesystem::path& path() const;

private:
    std::filesystem::path config_path_;
};

}
