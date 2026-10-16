#include "pomodoro/session_history.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_missing_file_returns_empty_history(
    const std::filesystem::path& directory
) {
    pomodoro::SessionHistory history{directory / "missing.json"};

    require(
        history.load().empty(),
        "A missing history file should return an empty history"
    );
}

void test_completed_sessions_are_saved_and_loaded(
    const std::filesystem::path& directory
) {
    const auto path = directory / "history.json";

    pomodoro::SessionHistory history{path};

    history.record_completed(
        pomodoro::SessionType::Work,
        std::chrono::minutes{25}
    );

    history.record_completed(
        pomodoro::SessionType::ShortBreak,
        std::chrono::minutes{5}
    );

    const auto records = history.load();

    require(records.size() == 2, "Expected two saved sessions");

    require(
        records[0].type == pomodoro::SessionType::Work,
        "The first session should be Work"
    );

    require(
        records[0].duration_seconds == 1500,
        "Work duration should be 1500 seconds"
    );

    require(
        records[1].type == pomodoro::SessionType::ShortBreak,
        "The second session should be Short Break"
    );

    require(
        records[1].duration_seconds == 300,
        "Short Break duration should be 300 seconds"
    );

    require(
        !records[0].completed_at.empty(),
        "A completion timestamp should be saved"
    );
}

void test_corrupted_json_is_rejected(
    const std::filesystem::path& directory
) {
    const auto path = directory / "corrupted.json";

    {
        std::ofstream file{path};
        file << "{ invalid json";
    }

    pomodoro::SessionHistory history{path};

    bool threw = false;

    try {
        static_cast<void>(history.load());
    } catch (const std::runtime_error&) {
        threw = true;
    }

    require(threw, "Corrupted JSON should throw an error");
}

void test_invalid_duration_is_rejected(
    const std::filesystem::path& directory
) {
    pomodoro::SessionHistory history{directory / "invalid.json"};

    bool threw = false;

    try {
        history.record_completed(
            pomodoro::SessionType::Work,
            std::chrono::seconds{0}
        );
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    require(threw, "A zero-length session should be rejected");
}

}

int main() {
    const auto directory =
        std::filesystem::temp_directory_path() /
        "pomodoro-cli-session-history-test";

    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    try {
        test_missing_file_returns_empty_history(directory);
        test_completed_sessions_are_saved_and_loaded(directory);
        test_corrupted_json_is_rejected(directory);
        test_invalid_duration_is_rejected(directory);

        std::filesystem::remove_all(directory);

        std::cout << "All Session History tests passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::filesystem::remove_all(directory);

        std::cerr << "Test failed: " << error.what() << '\n';
        return 1;
    }
}
