
#include "pomodoro/config_manager.hpp"
#include "pomodoro/session_history.hpp"
#include "pomodoro/session_manager.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <iostream>

namespace {

using namespace ftxui;

std::filesystem::path get_config_path() {
    if (const char* value = std::getenv("XDG_CONFIG_HOME")) {
        if (*value != '\0') {
            return std::filesystem::path{value}
                / "pomodoro-cli" / "config.json";
        }
    }

    if (const char* home = std::getenv("HOME")) {
        if (*home != '\0') {
            return std::filesystem::path{home}
                / ".config" / "pomodoro-cli" / "config.json";
        }
    }

    throw std::runtime_error("Could not determine config directory");
}

std::filesystem::path get_history_path() {
    if (const char* value = std::getenv("XDG_DATA_HOME")) {
        if (*value != '\0') {
            return std::filesystem::path{value}
                / "pomodoro-cli" / "history.json";
        }
    }

    if (const char* home = std::getenv("HOME")) {
        if (*home != '\0') {
            return std::filesystem::path{home}
                / ".local" / "share"
                / "pomodoro-cli" / "history.json";
        }
    }

    throw std::runtime_error("Could not determine data directory");
}

const char* session_name(pomodoro::SessionType type) {
    switch (type) {
        case pomodoro::SessionType::Work:
            return "WORK";

        case pomodoro::SessionType::ShortBreak:
            return "SHORT BREAK";

        case pomodoro::SessionType::LongBreak:
            return "LONG BREAK";
    }

    return "UNKNOWN";
}

pomodoro::TimerEngine::Duration duration_for(
    pomodoro::SessionType type,
    const pomodoro::SessionDurations& durations
) {
    switch (type) {
        case pomodoro::SessionType::Work:
            return durations.work;

        case pomodoro::SessionType::ShortBreak:
            return durations.short_break;

        case pomodoro::SessionType::LongBreak:
            return durations.long_break;
    }

    throw std::invalid_argument("Invalid session type");
}

std::string format_time(pomodoro::TimerEngine::Duration duration) {
    const auto total = std::max<long long>(0, duration.count());
    const auto minutes = total / 60;
    const auto seconds = total % 60;

    std::ostringstream output;
    output << std::setfill('0')
           << std::setw(2) << minutes
           << ':'
           << std::setw(2) << seconds;

    return output.str();
}

std::size_t completed_work_count(
    const std::vector<pomodoro::SessionRecord>& records
) {
    return static_cast<std::size_t>(
        std::count_if(
            records.begin(),
            records.end(),
            [](const pomodoro::SessionRecord& record) {
                return record.type == pomodoro::SessionType::Work;
            }
        )
    );
}

long long total_focus_seconds(
    const std::vector<pomodoro::SessionRecord>& records
) {
    long long total = 0;

    for (const auto& record : records) {
        if (record.type == pomodoro::SessionType::Work) {
            total += record.duration_seconds;
        }
    }

    return total;
}

std::string format_focus_time(long long seconds) {
    const auto minutes = seconds / 60;
    const auto hours = minutes / 60;
    const auto remaining_minutes = minutes % 60;

    if (hours > 0) {
        return std::to_string(hours) + "h "
            + std::to_string(remaining_minutes) + "m";
    }

    return std::to_string(minutes) + " min";
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
        }

        pomodoro::SessionHistory history{get_history_path()};
        auto records = history.load();

        pomodoro::SessionManager manager{config.durations};

        auto active_type = manager.current_session_type();
        auto active_duration = duration_for(active_type, config.durations);

        manager.start();

        bool session_active = true;
        std::string status = "FOCUS IN PROGRESS";

        auto screen = ScreenInteractive::Fullscreen();

        auto exit_loop = screen.ExitLoopClosure();
        std::atomic<bool> stop_ticker{false};

        auto renderer = Renderer([&] {
            const auto remaining = manager.remaining();

            const auto total_seconds =
                std::max<long long>(1, active_duration.count());

            const auto remaining_seconds =
                std::clamp<long long>(
                    remaining.count(), 0, total_seconds
                );

            const float progress = static_cast<float>(
                total_seconds - remaining_seconds
            ) / static_cast<float>(total_seconds);

            const auto work_count = completed_work_count(records);
            const auto focus_seconds = total_focus_seconds(records);

            const auto status_color =
                session_active ? Color::Green : Color::Yellow;

            const auto timer_color =
                active_type == pomodoro::SessionType::Work
                    ? Color::Cyan
                    : Color::Green;

            return vbox({
                text(" POMODORO CLI ") | bold | color(Color::Cyan)
                    | center,
                text("Build. Focus. Take a break. Repeat.")
                    | dim | center,
                separator(),
                text(std::string{"  "} + session_name(active_type))
                    | bold | color(status_color) | center,
                text(format_time(remaining))
                    | bold | color(timer_color) | center
                    | size(WIDTH, GREATER_THAN, 24),
                gauge(progress) | color(timer_color)
                    | size(WIDTH, GREATER_THAN, 30),
                text(session_active
                    ? (manager.is_running()
                        ? "  " + status
                        : "  PAUSED")
                    : "  SESSION COMPLETE — PRESS SPACE TO CONTINUE")
                    | center,
                separator(),
                hbox({
                    text("  Completed Work: ") | dim,
                    text(std::to_string(work_count))
                        | bold | color(Color::Cyan),
                    filler(),
                    text("Focus Time: ") | dim,
                    text(format_focus_time(focus_seconds))
                        | bold | color(Color::Cyan),
                    text("  ")
                }),
                separator(),
                text("  SPACE  Pause / Resume / Start next session"),
                text("  R      Reset current session"),
                text("  Q      Quit"),
                separator(),
                text("  History: " + std::to_string(records.size())
                    + " completed sessions") | dim,
            }) | border;
        });

        auto component = CatchEvent(renderer, [&](Event event) {
            if (event == Event::Character('q')
                || event == Event::Character('Q')) {
                stop_ticker.store(true);
                exit_loop();
                return true;
            }

            if (event == Event::Character(' ')) {
                if (!session_active) {
                    active_type = manager.current_session_type();
                    active_duration =
                        duration_for(active_type, config.durations);

                    manager.start();
                    session_active = true;
                    status = "SESSION STARTED";
                } else if (manager.is_running()) {
                    manager.pause();
                    status = "PAUSED";
                } else {
                    manager.resume();
                    status = "FOCUS IN PROGRESS";
                }

                return true;
            }

            if (event == Event::Character('r')
                || event == Event::Character('R')) {
                const bool was_active = session_active;

                manager.reset();
                session_active = false;
                status = "READY";

                if (was_active) {
                    manager.start();
                    session_active = true;
                    status = "SESSION RESET";
                }

                return true;
            }

            if (event == Event::Custom) {
                if (session_active && manager.is_finished()) {
                    history.record_completed(
                        active_type,
                        active_duration
                    );

                    records = history.load();
                    manager.complete_session();

                    session_active = false;
                    status = "SESSION COMPLETE";
                }

                return true;
            }

            return false;
        });

        std::jthread ticker([&](std::stop_token stop_token) {
            while (!stop_token.stop_requested()
                   && !stop_ticker.load()) {
                std::this_thread::sleep_for(200ms);

                if (!stop_token.stop_requested()
                    && !stop_ticker.load()) {
                    screen.PostEvent(Event::Custom);
                }
            }
        });

        screen.Loop(component);

        stop_ticker.store(true);
        ticker.request_stop();

        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
