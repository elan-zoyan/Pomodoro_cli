# pomodoro-cli

A terminal-based Pomodoro timer built with modern C++20. The goal is to create a lightweight, configurable productivity tool for Linux.

## Features

- Configurable work and break durations
- Start, pause, resume, and reset controls
- Short and long break sessions
- Session history and productivity statistics
- JSON-based configuration and persistence
- Sound notifications when sessions finish
- Interactive terminal user interface

Some features are planned and are not implemented yet.

## Tech Stack

- C++20
- CMake
- FTXUI
- nlohmann/json
- CTest

## Requirements

- A C++20-compatible compiler
- CMake 3.20 or newer
- Git

## Build

Clone the repository and enter the project directory:

```bash
git clone <repository-url>
cd pomodoro-cli
```

Configure the project:

```bash
cmake -S . -B build
```

Build the executable:

```bash
cmake --build build
```

Run the application:

```bash
./build/pomodoro-cli
```

## Development Status

The project is under active development.

Current focus:
- Timer engine
- Pause and resume behavior
- Automated tests

The planned configuration and application data locations follow the XDG Base Directory specification:

- Configuration: `$XDG_CONFIG_HOME/pomodoro-cli/`
- Application data: `$XDG_DATA_HOME/pomodoro-cli/`

When these environment variables are unset, the application will use the standard defaults under the user's home directory.

## Contributing

Contributions, bug reports, and suggestions are welcome.

## License

The project license has not been selected yet.
