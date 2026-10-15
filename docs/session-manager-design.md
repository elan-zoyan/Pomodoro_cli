# Session Manager Design

## Purpose

The Session Manager coordinates Pomodoro work and break sessions. It decides which session comes next and tracks completed work sessions.

## Session Types

- Work: 25 minutes by default
- Short Break: 5 minutes by default
- Long Break: 15 minutes by default

Durations must eventually be configurable.

## Session Rules

1. The application starts with a Work session.
2. Completing a Work session increments the completed work-session count.
3. After four completed Work sessions, the next session is a Long Break.
4. Otherwise, the next session is a Short Break.
5. Completing a break schedules the next Work session.
6. The application does not automatically start the next session.
7. The user must manually start the next session.

## Responsibilities

- Track the current session type.
- Track completed work sessions.
- Determine the next session type.
- Coordinate with Timer Engine.
- Reset the timer to the selected session's duration.
- Keep session scheduling separate from terminal UI code.

## Important Rules

- Paused or reset sessions do not count as completed.
- Only a genuinely completed Work session increments the count.
- Completing a break must not increment the work-session count.
- The long-break interval should be configurable in a future version.
- The next session remains stopped until the user starts it.

## Testing Plan

- The initial session is Work.
- Completing Work schedules Short Break when the long-break interval is not reached.
- Completing the fourth Work session schedules Long Break.
- Completing a break schedules Work.
- Incomplete sessions do not count as completed.
- The next session does not start automatically.
- Session durations match the configured values.

