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

# Set SDL2_image path
set(SDL2_IMAGE_PATH vendors/SDL_Image)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${SDL2_IMAGE_PATH})
include_directories(${SDL2_IMAGE_PATH}/include)
link_directories(${SDL2_IMAGE_PATH}/lib)
file(COPY ${SDL2_IMAGE_PATH}/bin/SDL2_image.dll DESTINATION ${CMAKE_BINARY_DIR})

# set SDL_ttf path
set(SDL2_TTF_PATH vendors/SDL_ttf)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${SDL2_TTF_PATH})
include_directories(${SDL2_TTF_PATH}/include)
link_directories(${SDL2_TTF_PATH}/lib)
file(COPY ${SDL2_TTF_PATH}/bin/SDL2_ttf.dll DESTINATION ${CMAKE_BINARY_DIR})

# set SDL2_mixer path
set(SDL2_MIXER_PATH vendors/SDL_mixer)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${SDL2_MIXER_PATH})
include_directories(${SDL2_MIXER_PATH}/include)
link_directories(${SDL2_MIXER_PATH}/lib)
file(COPY ${SDL2_MIXER_PATH}/bin/SDL2_mixer.dll DESTINATION ${CMAKE_BINARY_DIR})

# set GLM path
set(GLM_PATH vendors/glm)
include_directories(${GLM_PATH}/include)

# set simdjson path
set(SIMDJSON_PATH vendors/simdjson)
include_directories(${SIMDJSON_PATH}/include)
link_directories(${SIMDJSON_PATH}/lib)

# set imgui path
set(IMGUI_PATH vendors/imgui)
include_directories(${IMGUI_PATH})
set(IMGUI_FILES
        ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imgui_demo.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_tables.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp
        ${IMGUI_PATH}/backends/imgui_impl_sdl2.cpp
        ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer2.cpp
)

# Copy all files in the resources folder
file(GLOB RESOURCES_FILES ${CMAKE_SOURCE_DIR}/resources/*)
file(COPY ${RESOURCES_FILES} DESTINATION ${CMAKE_BINARY_DIR}/resources)

add_executable(SharkCardGame src/main.cpp
        src/scenes/PlayScene.cpp
        src/scenes/PlayScene.h
        src/scenes/MenuScene.cpp
        src/scenes/MenuScene.h
        src/components/Components.h
        src/Global.h
)

# Add the ENABLE_EDITOR definition for debug builds
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DENABLE_EDITOR)
endif ()

# Add ImGui files conditionally
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_sources(SharkCardGame PRIVATE ${IMGUI_FILES})
endif ()

target_link_libraries(SharkCardGame PRIVATE mingw32 Cbit2d SDL2::SDL2main SDL2::SDL2 SDL2_image SDL2_ttf SDL2_mixer spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32> pthread)

# Set linker flags for console application
set_target_properties(SharkCardGame PROPERTIES
        LINK_FLAGS "-mconsole"
)