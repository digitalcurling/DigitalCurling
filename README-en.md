# DigitalCurling3

- [README (Japanese Version)](./README.md)
- [Manual](https://github.com/digitalcurling/DigitalCurling3/wiki)
- [Official site](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)

Digital Curling is a curling simulation system for creating curling AI.

## Overview

This repository provides a library for curling AI development.
See [manual](https://github.com/digitalcurling/DigitalCurling3/wiki) for details.

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

1. Install Git
1. Install [CMake](https://cmake.org/)
1. Ensure CMake is in the user `PATH`
1. Clone this repository
1. To set up submodules, execute `git submodule update --init --recursive`
1. Execute the following commands for building

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

:warning: Only the static library is supported.

:warning: Installation is not supported now.

## Versioning

Compliant with [Semantic Versioning 2.0.0](https://semver.org/) about API compatibility.

- Major version is incremented if incompatible API changes occur.
- Minor version is incremented if functionality is added in a API backwards compatible manner.
- Patch version is incremented if API backwards compatible bug fix is done.

The API backwards compatible update guarantees the operation of software (applications and libraries) linked to the library.
This means not only that there will be no compile errors when updating the library, but also that the linked software will not behave improperly or the execution speed of library functions will not be degraded.

:warning: ABI compatibility is not considered due to development costs. Therefore, this library is provided as a static library only.

## License

MIT License
