cmake_minimum_required(VERSION 3.28)

project(SharkCardGame)

set(CMAKE_CXX_STANDARD 17)

# set entt path
include_directories(ENTT vendors/entt)

# set spdlog path
set(spdlog_PATH vendors/spdlog)
include_directories(${spdlog_PATH}/include)
link_directories(${spdlog_PATH}/lib)

# Set CMAKE_PREFIX_PATH to include spdlog_DIR
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${spdlog_PATH})

find_package(spdlog REQUIRED)

# set sdl2 path
set(SDL2_PATH vendors/SDL2)
include_directories(${SDL2_PATH}/include)
link_directories(${SDL2_PATH}/lib)
file(COPY ${SDL2_PATH}/bin/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR})

# Specify the path to the shared library
#set(console_PATH vendors/console)
#include_directories(${console_PATH}/include)
#link_directories(${console_PATH}/lib)
#file(COPY ${console_PATH}/bin/libprinting.dll DESTINATION ${CMAKE_BINARY_DIR})

add_executable(SharkCardGame src/main.cpp
        src/components/Player.cpp
        src/components/Player.h
        src/components/Card.cpp
        src/components/Card.h
        src/components/Deck.cpp
        src/components/Deck.h
        src/utilities/Logger.cpp
        src/utilities/Logger.h
        src/core/Application.cpp
        src/core/Application.h
)

target_link_libraries(SharkCardGame PRIVATE mingw32 SDL2main SDL2 spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32> pthread)
