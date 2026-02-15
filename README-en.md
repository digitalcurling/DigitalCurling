# DigitalCurling

- [README (Japanese Version)](./README.md)
- [Official Website](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)

DigitalCurling is a platform for simulating curling and developing curling AI.
WIts modular design allows for the extension of simulators and players (AI) as plugins.

## Overview

This repository provides libraries and an SDK for developing curling AI and simulators.
The system consists of `Core`, `Plugin API`, `Plugin Loader`, and standard `Plugins` (Simulator/Player).

### Specifications

- **Curling Reproduction**
  - Curling simulator that behaves closely to actual measurement data (FCV1 model, etc.)
  - Support for 4-person and 2-person (Mixed Doubles) rules
- **Multi-platform Support**
  - Windows 10/11
  - macOS
  - Linux (Ubuntu 20.04/22.04 LTS, etc.)

## Build

### Requirements

#### Tools
- C++17 compatible compiler (GCC, Clang, MSVC, etc.)
- Git
- [CMake](https://cmake.org/) 3.26 or higher
- [Doxygen](https://www.doxygen.nl/) (Optional: only if `DIGITALCURLING_BUILD_DOCS` is enabled)

#### Dependencies
The following libraries are automatically resolved by CMake:
- [nlohmann/json](https://github.com/nlohmann/json)
- [cpp-uuidv7](https://github.com/chromeru0312/cpp-uuidv7)
- [googletest](https://github.com/google/googletest) (Optional: only if `DIGITALCURLING_BUILD_TEST` is enabled)

### Build Options
You can customize the build configuration by specifying the following options when running CMake.

| オプション名 | デフォルト値 | 説明 |
| :--- | :--- | :--- |
| `DIGITALCURLING_BUILD_PLAYERS` | `ON` | Builds standard player plugins (Identical, NormalDist). |
| `DIGITALCURLING_BUILD_SIMULATORS` | `ON` | Builds standard simulator plugin (FCV1). |
| `DIGITALCURLING_BUILD_PLUGIN_LOADER` | `ON` | Builds the plugin loader library. |
| `DIGITALCURLING_PLUGIN_LOADER_SHARED` | *`OFF`* *1 | Builds the `plugin-loader` as a shared library. If `OFF`, it will be a static library. |
| `DIGITALCURLING_BUNDLE_PLUGINS` | `OFF` | If `ON`, plugins are statically linked (bundled) into the `plugin-loader`. Modules for dynamic loading are not built. |
| `DIGITALCURLING_PLUGIN_OUTPUT_DIR` | `"plugins"` | Specifies the output destination for plugin modules as a relative path from the build directory. |
| `DIGITALCURLING_BUILD_TEST` | `OFF` | Builds unit tests. Enabling this will automatically download GoogleTest. |
| `DIGITALCURLING_BUILD_DOCS` | `OFF` | Adds documentation generation targets (requires Doxygen). |

> *1: The default value of `DIGITALCURLING_PLUGIN_LOADER_SHARED` follows the setting of the CMake standard variable `BUILD_SHARED_LIBS` (usually `OFF`).

### Instructions

Since this project uses CMake's FetchContent feature, no prior submodule configuration is required.

1. Clone this repository.
   ```bash
   git clone https://github.com/digitalcurling/DigitalCurling.git
   cd DigitalCurling
   ```

1. Run the build using a preset.
   ```bash
   # Windows case (select the appropriate preset)
   cmake --preset windows-x64

   # Install
   cmake --install . --prefix /path/to/install
   ```

## Architecture and Directory Structure

- **src/core**: Basic definitions such as coordinate systems, game rules, data structures
- **src/plugin-api**: Headers required for plugin development (SDK)
- **src/plugin-loader**: Library for loading and managing plugins
- **src/player**: Implementation of player (AI) plugins
- **src/simulator**: Implementation of simulator plugins

## Versioning

We adhere to [Semantic Versioning 2.0.0](https://semver.org/).

- **Major version**: Updates incompatible with API backward compatibility
- **Minor version**: Feature additions compatible with API backward compatibility
- **Patch version**: Bug fixes


> [!WARNING]
> Due to C++ specifications, ABI compatibility may not be maintained if compiler versions or standard library versions differ.  
> When using dynamic loading (DLL/Shared Library), it is strongly recommended to build the main application and plugins in the exact same environment.  
> To avoid issues caused by version mismatches, please use DIGITALCURLING_BUNDLE_PLUGINS=ON to perform static linking.

## License

[MIT License](./LICENSE).
