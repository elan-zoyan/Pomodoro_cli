
#include "pomodoro/config_manager.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace std::chrono_literals;

namespace {

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::cout << "PASS: " << message << '\n';
}

template <typename Function>
bool throws_exception(Function&& function) {
    try {
        function();
    } catch (const std::exception&) {
        return true;
    }

    return false;
}

}

int main() {
    using pomodoro::AppConfig;
    using pomodoro::ConfigManager;

    const auto test_directory =
        std::filesystem::temp_directory_path()
        / "pomodoro-cli-config-test";

    std::filesystem::remove_all(test_directory);
    std::filesystem::create_directories(test_directory);

    const auto config_path = test_directory / "config.json";
    ConfigManager manager{config_path};

    const auto defaults = manager.load();

    check(
        defaults.durations.work == 25min,
        "Default Work duration is 25 minutes"
    );

    check(
        defaults.durations.short_break == 5min,
        "Default Short Break duration is 5 minutes"
    );

    check(
        defaults.durations.long_break == 15min,
        "Default Long Break duration is 15 minutes"
    );

    check(
        defaults.durations.long_break_interval == 4,
        "Default Long Break interval is 4"
    );

    AppConfig config;
    config.durations.work = 30min;
    config.durations.short_break = 7min;
    config.durations.long_break = 20min;
    config.durations.long_break_interval = 3;
    config.sound_enabled = false;

    manager.save(config);

    check(
        std::filesystem::exists(config_path),
        "Saving creates the configuration file"
    );

    const auto loaded = manager.load();

    check(
        loaded.durations.work == 30min,
        "Saved Work duration loads correctly"
    );

    check(
        loaded.durations.short_break == 7min,
        "Saved Short Break duration loads correctly"
    );

    check(
        loaded.durations.long_break == 20min,
        "Saved Long Break duration loads correctly"
    );

    check(
        loaded.durations.long_break_interval == 3,
        "Saved Long Break interval loads correctly"
    );

    check(
        !loaded.sound_enabled,
        "Saved sound setting loads correctly"
    );

    AppConfig invalid_config;
    invalid_config.durations.long_break_interval = 0;

    check(
        throws_exception([&] {
            manager.save(invalid_config);
        }),
        "Zero Long Break interval is rejected"
    );

    {
        std::ofstream output(config_path, std::ios::trunc);
        output << "{ invalid json";
    }

    check(
        throws_exception([&] {
            manager.load();
        }),
        "Corrupted JSON is rejected"
    );

    std::filesystem::remove_all(test_directory);

    std::cout << "All ConfigManager tests completed.\n";

    return EXIT_SUCCESS;
}

