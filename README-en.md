# Digital Curling :curling_stone: version 3

- [README (Japanese Version)](./README.md)
- [Official site](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)

Digital Curling is a curling simulation system for creating curling AI.

## Overview

### What is Provided in this Repository

- Library for developing a curling AI
- TCP/IP server for playing a curling match

### Curling Simulation

- Using a new physics simulator
- Currently normal rule (non mix-doubles rule) is only supported.

### Library

- Language: C++17
- Build Tool: CMake
- Multi-platform support
  - Tested in Windows and Ubuntu--18.04 LTS (WSL2)
  - Not tested in Mac, but we plan to do so.

## Building

1. Clone this repository
1. To set up submodules, execute `git submodule update --init --recursive`
1. Install [Boost](https://www.boost.org/)
1. Install [CMake](https://cmake.org/)
1. Ensure CMake is in the user `PATH`
1. Execute the following commands

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBOX2D_BUILD_TESTBED=OFF ..
cmake --build . --config RelWithDebInfo
```

:warning: If CMake could not find Boost, set the environment variable `BOOST_ROOT` to the directory in which Boost installed.

:warning: Currently, only the static library is supported. Also, installation is not supported.

## Creating a Curling AI

TODO

## Playing a Curling Match

- Build the server. See [here](#building).
- Make a server config file. See [here](#server-config-file).
- Execute the command below (the command and its location is slightly different for each platform). The first argument is the path to a config file. In the example, the name of the config file is `config.json`.

```
./digital_curling__server config.json
```

### Server config file

TODO

## GUI

TODO

## Versioning

Supports semantic versioning.

- Major version: Incremented when the library compatibility is broken.
- Minor version: Incremented when new features are added, but the compatibility is not broken.
- Patch version: Incremented when bug fixed.

:warning: Binary compatibility is not guaranteed for major or minor version updates (to reduce development costs). Binary compatibility is guaranteed when the patch version is upgraded.

# License

MIT License
