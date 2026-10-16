
#include "pomodoro/config_manager.hpp"
#include "pomodoro/session_manager.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

namespace {

std::filesystem::path get_config_path() {
    if (const char* xdg_config = std::getenv("XDG_CONFIG_HOME")) {
        if (*xdg_config != '\0') {
            return std::filesystem::path{xdg_config}
                / "pomodoro-cli"
                / "config.json";
        }
    }

    if (const char* home = std::getenv("HOME")) {
        if (*home != '\0') {
            return std::filesystem::path{home}
                / ".config"
                / "pomodoro-cli"
                / "config.json";
        }
    }

    throw std::runtime_error(
        "Could not determine the configuration directory"
    );
}

const char* session_name(pomodoro::SessionType type) {
    switch (type) {
        case pomodoro::SessionType::Work:
            return "Work";

        case pomodoro::SessionType::ShortBreak:
            return "Short Break";

        case pomodoro::SessionType::LongBreak:
            return "Long Break";
    }

    return "Unknown";
}

void display_remaining(pomodoro::TimerEngine::Duration remaining) {
    const auto total_seconds = remaining.count();
    const auto minutes = total_seconds / 60;
    const auto seconds = total_seconds % 60;

    std::cout << "\rTime remaining: "
              << std::setfill('0')
              << std::setw(2) << minutes
              << ':'
              << std::setw(2) << seconds
              << "   "
              << std::flush;
}

}

int main() {
    using namespace std::chrono_literals;

    try {
        const auto config_path = get_config_path();
        pomodoro::ConfigManager config_manager{config_path};

        const bool config_exists =
            std::filesystem::exists(config_path);

        const auto config = config_manager.load();

        if (!config_exists) {
            config_manager.save(config);

            std::cout
                << "Created default configuration at:\n"
                << config_path
                << "\n\n";
        }

        pomodoro::SessionManager manager{config.durations};

        std::cout << "Pomodoro CLI\n";
        std::cout << "Session: "
                  << session_name(manager.current_session_type())
                  << '\n';

        std::cout << "Completed work sessions: "
                  << manager.completed_work_sessions()
                  << "\n\n";

        manager.start();

        while (!manager.is_finished()) {
            display_remaining(manager.remaining());
            std::this_thread::sleep_for(200ms);
        }

        display_remaining(0s);
        std::cout << "\n\nSession completed!\n";

        if (manager.complete_session()) {
            std::cout << "Next session: "
                      << session_name(manager.current_session_type())
                      << '\n';

            std::cout
                << "The next session has not been started.\n";
        }

        return 0;
    } catch (const std::exception& error) {
        std::cerr << "\nError: " << error.what() << '\n';
        return 1;
    }
}

