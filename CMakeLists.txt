cmake_minimum_required(VERSION 3.28)

project(SharkCardGame)

set(CMAKE_CXX_STANDARD 17)

# set entt path
include_directories(ENTT vendors/entt)

# set spdlog path
if (NOT TARGET spdlog)
    # Stand-alone build
    set(spdlog_DIR vendors/spdlog/include)
    find_package(spdlog REQUIRED)
endif ()

add_executable(SharkCardGame src/main.cpp
        src/components/Player.cpp
        src/components/Player.h
        src/components/Card.cpp
        src/components/Card.h
        src/components/Deck.cpp
        src/components/Deck.h
)

target_link_libraries(SharkCardGame PRIVATE spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)
