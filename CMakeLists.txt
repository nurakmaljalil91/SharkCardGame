cmake_minimum_required(VERSION 3.28)

project(SharkCardGame)

set(CMAKE_CXX_STANDARD 17)

# set cbit2d path
set(CBIT2D_PATH vendors/cbit2d)
include_directories(${CBIT2D_PATH}/include)
link_directories(${CBIT2D_PATH}/lib)
file(COPY ${CBIT2D_PATH}/bin/libCbit2d.dll DESTINATION ${CMAKE_BINARY_DIR})

# set entt path
include_directories(vendors/entt)

# set spdlog path
set(spdlog_PATH vendors/spdlog)
include_directories(${spdlog_PATH}/include)
link_directories(${spdlog_PATH}/lib)

# Set CMAKE_PREFIX_PATH to include spdlog_DIR
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${spdlog_PATH})

find_package(spdlog REQUIRED)

# Set SDL2 path
set(SDL2_PATH vendors/SDL2)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${SDL2_PATH})
include_directories(${SDL2_PATH}/include)
link_directories(${SDL2_PATH}/lib)
file(COPY ${SDL2_PATH}/bin/SDL2.dll DESTINATION ${CMAKE_BINARY_DIR})

# Find SDL2 package
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(SharkCardGame src/main.cpp
        src/components/Player.cpp
        src/components/Player.h
        src/components/Card.cpp
        src/components/Card.h
        src/components/Deck.cpp
        src/components/Deck.h
        src/scenes/CustomScene.cpp
        src/scenes/CustomScene.h
)

target_link_libraries(SharkCardGame PRIVATE mingw32 Cbit2d SDL2::SDL2main SDL2::SDL2 spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32> pthread)

# Set linker flags for console application
set_target_properties(SharkCardGame PROPERTIES
        LINK_FLAGS "-mconsole"
)