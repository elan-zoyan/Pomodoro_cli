# Testing Plan

## Goals

Verify that the Timer Engine behaves correctly across normal operations, invalid inputs, and state transitions.

## Unit Tests

### Construction
- Positive duration initializes the timer.
- Zero duration throws an exception.
- Negative duration throws an exception.

### Start and Countdown
- Starting a ready timer activates the countdown.
- Remaining time decreases while running.
- Starting an already running timer does not reset elapsed progress.

### Pause and Resume
- Pausing preserves the remaining duration.
- Remaining time stays stable while paused.
- Resuming continues from the saved remaining duration.
- Resuming an already running timer does not restart the countdown.

### Reset and Completion
- Reset restores the original duration.
- A finished timer reports zero remaining time.
- Remaining time never becomes negative.
- Reset allows the timer to start again.

## Test Infrastructure

Use CTest with a small C++ test executable. Tests should avoid unnecessary long sleeps. Timing-dependent tests should allow a reasonable tolerance for scheduler delays.

## Acceptance Criteria

- All tests pass.
- The project builds with the configured compiler warnings.
- Timer behavior matches the documented design.
