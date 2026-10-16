#pragma once

#include "pomodoro/session_manager.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace pomodoro {

struct SessionRecord {
    SessionType type;
    long long duration_seconds;
    std::string completed_at;
};

class SessionHistory {
public:
    explicit SessionHistory(std::filesystem::path history_path);

    std::vector<SessionRecord> load() const;

    void record_completed(
        SessionType type,
        TimerEngine::Duration duration
    ) const;

    const std::filesystem::path& path() const;

private:
    void save(const std::vector<SessionRecord>& records) const;

    std::filesystem::path history_path_;
};

}
