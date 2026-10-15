
#include "pomodoro/session_manager.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <thread>

using namespace std::chrono_literals;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::cout << "PASS: " << message << '\n';
}

void finish_current_session(pomodoro::SessionManager& manager) {
    manager.start();

    while (!manager.is_finished()) {
        std::this_thread::sleep_for(50ms);
    }

    check(
        manager.is_finished(),
        "Session reaches completion"
    );

    check(
        manager.complete_session(),
        "Completed session transitions successfully"
    );
}

int main() {
    using pomodoro::SessionDurations;
    using pomodoro::SessionManager;
    using pomodoro::SessionType;

    SessionDurations durations{
        .work = 1s,
        .short_break = 1s,
        .long_break = 2s,
        .long_break_interval = 2
    };

    SessionManager manager{durations};

    check(
        manager.current_session_type() == SessionType::Work,
        "Initial session is Work"
    );

    check(
        manager.remaining() == 1s,
        "Initial Work duration is correct"
    );

    check(
        !manager.complete_session(),
        "Incomplete session cannot be completed"
    );

    check(
        manager.completed_work_sessions() == 0,
        "Incomplete session does not increment count"
    );

    manager.start();
    manager.pause();
    manager.reset();

    check(
        manager.remaining() == 1s,
        "Reset restores Work duration"
    );

    check(
        manager.completed_work_sessions() == 0,
        "Reset does not increment completed count"
    );

    finish_current_session(manager);

    check(
        manager.current_session_type() == SessionType::ShortBreak,
        "First Work session schedules Short Break"
    );

    check(
        manager.completed_work_sessions() == 1,
        "First Work session increments count"
    );

    check(
        !manager.is_running(),
        "Next session remains stopped"
    );

    finish_current_session(manager);

    check(
        manager.current_session_type() == SessionType::Work,
        "Short Break schedules Work"
    );

    check(
        manager.completed_work_sessions() == 1,
        "Break does not increment Work count"
    );

    finish_current_session(manager);

    check(
        manager.current_session_type() == SessionType::LongBreak,
        "Second Work session schedules Long Break"
    );

    check(
        manager.completed_work_sessions() == 2,
        "Second Work session increments count"
    );

    check(
        manager.remaining() == 2s,
        "Long Break duration is correct"
    );

    finish_current_session(manager);

    check(
        manager.current_session_type() == SessionType::Work,
        "Long Break schedules Work"
    );

    check(
        manager.remaining() == 1s,
        "Next Work duration is restored"
    );

    bool rejected_invalid_interval = false;

    try {
        SessionDurations invalid_durations{
            .work = 1s,
            .short_break = 1s,
            .long_break = 2s,
            .long_break_interval = 0
        };

        SessionManager invalid_manager{invalid_durations};
    } catch (const std::invalid_argument&) {
        rejected_invalid_interval = true;
    }

    check(
        rejected_invalid_interval,
        "Zero long-break interval is rejected"
    );

    bool rejected_invalid_work_duration = false;

    try {
        SessionDurations invalid_durations{
            .work = 0s,
            .short_break = 1s,
            .long_break = 2s,
            .long_break_interval = 2
        };

        SessionManager invalid_manager{invalid_durations};
    } catch (const std::invalid_argument&) {
        rejected_invalid_work_duration = true;
    }

    check(
        rejected_invalid_work_duration,
        "Zero Work duration is rejected"
    );

    std::cout << "All Session Manager tests completed.\n";

    return EXIT_SUCCESS;
}

