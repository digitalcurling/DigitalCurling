add_executable(digital_curling__server
    log.cpp
    log.hpp
    main.cpp
    server.cpp
    server.hpp
)

# boost.asio
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.75 REQUIRED COMPONENTS date_time regex)

find_package(Threads REQUIRED)

target_link_libraries(digital_curling__server
  PRIVATE
    digital_curling::digital_curling
    digital_curling::json
    Boost::headers
    Boost::date_time
    Boost::regex
    Threads::Threads
)
