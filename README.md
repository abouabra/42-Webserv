# 42-Webserv

## Overview

**42-Webserv** is a custom web server project developed in C++. It is structured to process HTTP requests, serve static and dynamic content, and provide configurable server behavior using custom configuration files. The project is organized for clarity and modularity, making it accessible for both maintenance and extension.

## Project Structure

- **assets/**: Contains server-related resources and test data, organized into subfolders such as `server_1`, `server_2`, and `server_3`.
- **config/**: Holds server configuration files, e.g. `default.conf` and `test.conf`, to define server settings, routes, and other parameters.
- **includes/**: Header files (`*.hpp`) that define core classes and utilities, including:
    - `WebServ.hpp`, `Server.hpp`, `Client.hpp`, `Config.hpp`, `Utils.hpp`
- **src/**: Implementation files (`*.cpp`) corresponding to the project modules:
    - Server logic, client management, configuration parsing, utility functions, and entry point (`main.cpp`).
- **Makefile**: Build automation file for compiling and managing the project lifecycle.

## Features

- **Custom HTTP Server:** Implements core aspects of HTTP protocol (handling requests and responses).
- **Configurable:** Supports multiple configuration files to control server setup, routes, and behavior.
- **Modular Design:** Clean separation between server, client, configuration, and utility logic.
- **Multi-Server Support:** Designed to support running and testing several independent server instances concurrently, with separate asset directories for each instance.
- **Static Content Serving:** Serves assets and resources from designated directories.

## Getting Started

### Prerequisites

- C++ compatible compiler (e.g., `g++` or `clang++`)
- GNU Make

### Building the Project

```bash
make
```

### Running the Server

The server can be launched from the build output. You can specify a configuration file as needed:
```bash
  ./webserv config/default.conf
```


### Configuration

Edit the configuration files in the `config/` directory to adjust server ports, locations, error pages, and custom rules. These files determine how the server responds to requests and manages resources.

## Usage

- Place your web assets (HTML, CSS, JS, media) into the corresponding `assets/server_*` directories.
- Launch different instances or point to different configurations for varied test scenarios.

## Development

The codebase is modular, making contributions focused and manageable:
- Add new features in `src/` and declare structures in `includes/`.
- Update or add server behaviors via the configuration system.

## License

This project is developed as part of the 42-school curriculum and provided for educational purposes.
