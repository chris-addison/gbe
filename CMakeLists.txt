set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_minimum_required(VERSION 3.10)
project(gbe)

set(CMAKE_C_STANDARD 11)

include_directories(src)
include_directories(src/debug)
include_directories(src/frontend)
include_directories(src/gfx)
include_directories(src/opcodes)

if (WIN32)
    set(SDL2_DIR ${CMAKE_SOURCE_DIR}/externals/SDL2-2.0.8/x86_64-w64-mingw32)
endif ()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(gbe
        src/debug/debug.c
        src/debug/debug.h
        src/frontend/sdl/sdl.c
        src/frontend/frontend.h
        src/opcodes/cb_opcodes.c
        src/opcodes/opcodes.c
        src/opcodes/opcodes.h
        src/cartridge.c
        src/cartridge.h
        src/common.c
        src/common.h
        src/cpu.c
        src/cpu.h
        src/display.c
        src/display.h
        src/file.c
        src/gbe.c
        src/gbe.h
        src/input.h
        src/interrupts.c
        src/interrupts.h
        src/joypad.c
        src/joypad.h
        src/mbc.c
        src/mbc.h
        src/memory.c
        src/memory.h
        src/memory_map.h
        src/screen.c
        src/screen.h
        src/types.h
        src/window.h)

target_link_libraries(gbe ${SDL2_LIBRARIES})

