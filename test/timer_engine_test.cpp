#include "pomodoro/timer_engine.hpp"

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

int main() {
    using pomodoro::TimerEngine;

    TimerEngine timer{4s};

    check(!timer.is_running(), "Timer starts stopped");
    check(timer.remaining() == 4s, "Initial duration is correct");

    timer.start();

    check(timer.is_running(), "Timer starts");

    std::this_thread::sleep_for(1200ms);
    timer.pause();

    const auto paused_remaining = timer.remaining();

    check(!timer.is_running(), "Timer pauses");
    check(paused_remaining <= 3s, "Elapsed time is recorded");

    std::this_thread::sleep_for(300ms);

    check(
        timer.remaining() == paused_remaining,
        "Remaining time stays stable while paused"
    );

    timer.resume();

    check(timer.is_running(), "Timer resumes");

    const auto before_second_resume = timer.remaining();

    timer.resume();

    check(
        timer.remaining() <= before_second_resume,
        "Repeated resume does not restart the timer"
    );

    timer.pause();
    timer.reset();

    check(!timer.is_running(), "Reset stops timer");
    check(timer.remaining() == 4s, "Reset restores duration");

    timer.set_duration(2s);

    check(
        !timer.is_running(),
        "Changing duration stops the timer"
    );

    check(
        timer.remaining() == 2s,
        "Changing duration updates remaining time"
    );

    bool rejected_zero_duration = false;

    try {
        timer.set_duration(0s);
    } catch (const std::invalid_argument&) {
        rejected_zero_duration = true;
    }

    check(
        rejected_zero_duration,
        "Setting zero duration is rejected"
    );

    bool rejected_negative_duration = false;

    try {
        timer.set_duration(-1s);
    } catch (const std::invalid_argument&) {
        rejected_negative_duration = true;
    }

    check(
        rejected_negative_duration,
        "Setting negative duration is rejected"
    );

    bool rejected_zero_constructor = false;

    try {
        TimerEngine invalid{0s};
    } catch (const std::invalid_argument&) {
        rejected_zero_constructor = true;
    }

    check(
        rejected_zero_constructor,
        "Zero constructor duration is rejected"
    );

    bool rejected_negative_constructor = false;
 
    try {
        TimerEngine invalid{-1s};
    } catch (const std::invalid_argument&) {
        rejected_negative_constructor = true;
    }

    check(
        rejected_negative_constructor,
        "Negative constructor duration is rejected"
    );

    std::cout << "All tests completed.\n";

    return EXIT_SUCCESS;
}
