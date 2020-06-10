cmake_minimum_required(VERSION 3.10.2)
project(TicketSystem)
set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g")
include_directories(
        src
)
set(src_dir
        src/FileManager.hpp
        src/OrderManager.hpp
        src/UserManager.hpp
        src/timeType.hpp
        src/LRUCache.hpp
        src/map.hpp
        src/main.cpp
        src/TypesAndHeaders.hpp
        src/TrainManager.hpp src/TicketSystem.cpp src/StringHasher.hpp)
add_executable(TicketSystem ${src_dir})
