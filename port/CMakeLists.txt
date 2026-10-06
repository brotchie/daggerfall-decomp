# The native build of FALL.EXE's game code for the host (arm64 macOS first), with SDL3 for
# the platform (docs/port.md). Configure and build with tools/port_build.py, which also writes
# the generated sources in <build>/gen: definitions for the game's data and stubs for every
# symbol nothing defines yet.
cmake_minimum_required(VERSION 3.20)
project(dagger_port C)

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
find_package(SDL3 REQUIRED CONFIG)

# The game's C exactly as the matching build compiles it. Watcom 10.0a's dialect: C89 with
# its extensions, plain char unsigned, int and long 32 bits, signed overflow wrapping, and
# the type punning of decompiled code (no strict aliasing).
file(GLOB GAME_SRC CONFIGURE_DEPENDS
    ${ROOT}/src/lifted/*.c ${ROOT}/src/hand/*.c ${ROOT}/src/*.c)
add_library(game OBJECT ${GAME_SRC})
target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${ROOT}/include)
target_compile_options(game PRIVATE
    -std=gnu89 -include ${CMAKE_CURRENT_SOURCE_DIR}/include/port.h
    -funsigned-char -fwrapv -fno-strict-aliasing
    # FALL.EXE's x87 runs at 53-bit precision (control word 0x127F) and never fuses a
    # multiply and an add: each double operation rounds on its own, as here without FMA
    -ffp-contract=off
    # the build reports errors only; tools/port_census.py counts the warnings (the
    # worklist of docs/port.md). These would be errors in clang 16+ and stay warnings until
    # the declarations agree:
    -w -Wno-error=int-conversion -Wno-error=incompatible-function-pointer-types
    -Wno-error=implicit-function-declaration -Wno-error=implicit-int
    -Wno-error=return-type)

# The host side: the C library and MemCheck as the game expects them, DOS and DPMI services,
# and the SDL platform layer.
file(GLOB PORT_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shim/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/*.c)
file(GLOB GEN_SRC CONFIGURE_DEPENDS ${CMAKE_BINARY_DIR}/gen/*.c)

add_executable(fall ${PORT_SRC} ${GEN_SRC} $<TARGET_OBJECTS:game>)
target_include_directories(fall PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(fall PRIVATE -std=gnu11 -Wall -Wno-unused-function -ffp-contract=off)
target_link_libraries(fall PRIVATE SDL3::SDL3)
