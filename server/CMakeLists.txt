add_executable(digital_curling__server
    config.cpp
    config.hpp
    game.cpp
    game.hpp
    log.cpp
    log.hpp
    main.cpp
    server.cpp
    server.hpp
    tcp_session.cpp
    tcp_session.hpp
    trajectory_compressor.cpp
    trajectory_compressor.hpp
    util.cpp
    util.hpp
)

# boost.asio, boost.program_options
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.75 REQUIRED COMPONENTS date_time regex program_options)

find_package(Threads REQUIRED)

target_link_libraries(digital_curling__server
  PRIVATE
    digital_curling::digital_curling
    nlohmann_json::nlohmann_json
    Boost::headers
    Boost::date_time
    Boost::regex
    Boost::program_options
    Threads::Threads
)
