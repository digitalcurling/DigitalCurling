# Digital Curling :curling_stone: version 3

Digital Curling is a curling simulation platform for creating curling AI.

- [Official site](http://minerva.cs.uec.ac.jp/cgi-bin/curling/wiki.cgi)

## Feature

This repository provides...

- Library for developing a curling AI
- TCP/IP server for playing a curling match

### Curling Simulation

- Using a new physics simulator
- Currently normal rule (non mix-doubles rule) is only supported.

### Language

- C++17

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

### Installation

Currently not supported

## Creating a Curling AI

See [examples](https://github.com/digitalcurling/DigitalCurling-AI-example).

## Playing a Curling Match

- Build the server. See [here](#building).
- Make a server config file. See [here](#server-config-file).
- Execute the command below (the command and its location is slightly different for each platform). The first argument is the path to a config file. In the example, the name of the config file is `config.json`.

```
./digital_curling__server config.json
```

### Server config file

```json
{
    "port": [10000, 10001]
}
```

The detail of the config file is under construction.


## GUI

Under construction
