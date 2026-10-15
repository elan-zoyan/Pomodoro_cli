#pragma once

#include "pomodoro/timer_engine.hpp"

#include <cstddef>

namespace pomodoro {

enum class SessionType {
    Work,
    ShortBreak,
    LongBreak
};

struct SessionDurations {
    TimerEngine::Duration work{std::chrono::minutes{25}};
    TimerEngine::Duration short_break{std::chrono::minutes{5}};
    TimerEngine::Duration long_break{std::chrono::minutes{15}};
    std::size_t long_break_interval{4};
};

class SessionManager {
public:
    explicit SessionManager(
        SessionDurations durations = {}
    );

    void start();
    void pause();
    void resume();
    void reset();

    bool complete_session();

    TimerEngine::Duration remaining() const;
    bool is_running() const;
    bool is_finished() const;

    SessionType current_session_type() const;
    std::size_t completed_work_sessions() const;

private:
    TimerEngine::Duration duration_for(
        SessionType type
    ) const;

    SessionDurations durations_;
    TimerEngine timer_;

    SessionType current_type_ = SessionType::Work;
    std::size_t completed_work_sessions_ = 0;
};

}
