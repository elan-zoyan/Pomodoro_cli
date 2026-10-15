
#include "pomodoro/timer_engine.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

int main() {
    using namespace std::chrono_literals;

    pomodoro::TimerEngine timer{10s};

    timer.start();

    while (!timer.is_finished()) {
        const auto seconds = timer.remaining().count();

        std::cout << "\rTime remaining: "
                  << std::setfill('0')
                  << std::setw(2) << seconds / 60
                  << ":"
                  << std::setw(2) << seconds % 60
                  << std::flush;

        std::this_thread::sleep_for(200ms);
    }

    std::cout << "\rTimer finished!       \n";

    return 0;
}

