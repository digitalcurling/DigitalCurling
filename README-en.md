# Digital Curling :curling_stone: 3

- [README (Japanese Version)](./README.md)
- [Manual(TODO後でリンク貼る)]()
- [Official site](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)

Digital Curling is a curling simulation system for creating curling AI.

## Overview

This repository provides a library for curling AI development.
See [manual(TODO後でリンク貼る)]() for details.

### Specifications

- Curling simulation
  - Using a new physics simulator close to actual measurement data.
  - Currently normal rule (non mix-doubles rule) is only supported.
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
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

:warning: If CMake could not find Boost, set the environment variable `BOOST_ROOT` to the directory in which Boost installed.

:warning: Currently, only the static library is supported. Also, installation is not supported.

# License

MIT License
