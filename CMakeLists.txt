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
    src/core/session_manager.cpp
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

include(CTest)

if(BUILD_TESTING)
    add_executable(timer_engine_test
        test/timer_engine_test.cpp
        src/core/timer_engine.cpp
    )

    target_include_directories(timer_engine_test
        PRIVATE include
    )

    add_executable(session_manager_test
        test/session_manager_test.cpp
        src/core/timer_engine.cpp
        src/core/session_manager.cpp
    )

    target_include_directories(session_manager_test
        PRIVATE include
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(timer_engine_test PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )

        target_compile_options(session_manager_test PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )
    endif()

    add_test(
        NAME timer_engine_test
        COMMAND timer_engine_test
    )

    add_test(
        NAME session_manager_test
        COMMAND session_manager_test
    )
endif()
