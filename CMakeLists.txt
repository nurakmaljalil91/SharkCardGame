cmake_minimum_required(VERSION 3.27)

project(SharkCardGame)

set(CMAKE_CXX_STANDARD 17)

# set entt path
include_directories(ENTT vendors/entt)

add_executable(SharkCardGame main.cpp)
