cmake_minimum_required(VERSION 3.20)

project(pomodoro-cli
    VERSION 0.1.0
    DESCRIPTION "A terminal-based Pomodoro timer"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(pomodoro-cli
    src/main.cpp
    src/core/timer_engine.cpp
)

target_include_directories(pomodoro-cli
    PRIVATE include
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(pomodoro-cli PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

