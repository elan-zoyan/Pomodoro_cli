#pragma once

#include <chrono>

namespace pomodoro {

class TimerEngine {
public:
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::seconds;

    explicit TimerEngine(Duration duration);

    void start();
    void pause();
    void resume();
    void reset();

    Duration remaining() const;
    bool is_running() const;
    bool is_finished() const;

private:
    Duration duration_;
    Duration remaining_;

    Clock::time_point started_at_{};
    bool running_ = false;
};

}