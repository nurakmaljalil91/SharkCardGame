cmake_minimum_required(VERSION 3.28)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(_default_vcpkg_toolchain "C:/Users/User/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
    if(EXISTS "${_default_vcpkg_toolchain}")
        set(CMAKE_TOOLCHAIN_FILE "${_default_vcpkg_toolchain}" CACHE STRING "Vcpkg toolchain file")
    endif()
endif()

project(SharkCardGame)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../Cbit2d ${CMAKE_BINARY_DIR}/Cbit2d-build)

file(GLOB RESOURCES_FILES ${CMAKE_SOURCE_DIR}/resources/*)
file(COPY ${RESOURCES_FILES} DESTINATION ${CMAKE_BINARY_DIR}/resources)

add_executable(
    SharkCardGame
    src/main.cpp
    src/gameplay/gameplay_state.cpp
    src/gameplay/match_rules.cpp
    src/scenes/menu_scene.cpp
    src/scenes/play_scene.cpp
    src/scenes/play_scene_ui.cpp
    src/scenes/round_flow_controller.cpp
    src/scenes/table_layout.cpp
)

target_link_libraries(SharkCardGame PRIVATE cbit2d::cbit2d)

if(MINGW)
    target_compile_options(SharkCardGame PRIVATE -Wa,-mbig-obj)

    get_filename_component(_compiler_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(_mingw_runtime_dlls
        "${_compiler_bin_dir}/libstdc++-6.dll"
        "${_compiler_bin_dir}/libgcc_s_seh-1.dll"
        "${_compiler_bin_dir}/libwinpthread-1.dll"
    )

    foreach(_runtime_dll IN LISTS _mingw_runtime_dlls)
        if(EXISTS "${_runtime_dll}")
            add_custom_command(
                TARGET SharkCardGame POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_runtime_dll}"
                $<TARGET_FILE_DIR:SharkCardGame>
            )
        endif()
    endforeach()
endif()
