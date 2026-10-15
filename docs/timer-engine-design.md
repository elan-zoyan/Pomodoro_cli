# Timer Engine Design

## Purpose

The Timer Engine manages countdown timing independently of the terminal user interface. It is responsible for starting, pausing, resuming, resetting, and reporting the remaining duration.

## Responsibilities

- Track the remaining timer duration.
- Start and pause a countdown.
- Resume a paused countdown without losing elapsed progress.
- Reset the timer to its original duration.
- Report whether the timer is running or finished.
- Prevent the remaining duration from becoming negative.

## Timer States

| State | Description |
|---|---|
| Ready | Timer has not started or has been reset |
| Running | Countdown is active |
| Paused | Countdown is temporarily stopped |
| Finished | Remaining duration is zero |

## Public Interface

- `start()` starts a ready timer.
- `pause()` pauses a running timer.
- `resume()` resumes a paused timer.
- `reset()` restores the original duration.
- `remaining()` returns the remaining duration.
- `is_running()` reports whether the timer is running.
- `is_finished()` reports whether the countdown has completed.

## Timekeeping

Use `std::chrono::steady_clock` for elapsed-time calculations. It is monotonic and is not affected by ordinary system clock adjustments.

The engine should calculate elapsed time from clock timestamps instead of decrementing a counter on every UI refresh.

## Pause and Resume

When pausing, calculate the remaining duration and store it.

When resuming, use a new start timestamp and preserve the stored remaining duration. Previously elapsed time must not be counted twice.

## Reset

Reset must stop the countdown and restore the original duration.

## Edge Cases

- A zero or negative duration must be rejected.
- Pausing a non-running timer must not corrupt its state.
- Resuming an already running timer must not restart its countdown.
- A finished timer must not become negative.
- Resetting a paused or finished timer must restore the original duration.
- Calling `remaining()` must not change the timer state.

## Testing Plan

- Construct a timer with a positive duration.
- Reject zero and negative durations.
- Start a timer and verify that its remaining time decreases.
- Pause a timer and verify that its remaining time stays stable.
- Resume a paused timer and verify that the countdown continues.
- Reset a running, paused, and finished timer.
- Verify that a completed timer reports zero remaining time.

