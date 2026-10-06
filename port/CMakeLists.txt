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
# and the virtual PC on SDL3 (port/include/port_vpc.h). A static library, so the demo links
# the platform without the game.
file(GLOB PORT_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shim/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/host/*.c)
list(REMOVE_ITEM PORT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/host/main.c)
add_library(porthost STATIC ${PORT_SRC})
target_include_directories(porthost PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(porthost PRIVATE -std=gnu11 -Wall -Wno-unused-function -ffp-contract=off)
target_link_libraries(porthost PUBLIC SDL3::SDL3)

file(GLOB GEN_SRC CONFIGURE_DEPENDS ${CMAKE_BINARY_DIR}/gen/*.c)

add_executable(fall ${CMAKE_CURRENT_SOURCE_DIR}/host/main.c ${GEN_SRC} $<TARGET_OBJECTS:game>)
target_compile_options(fall PRIVATE -std=gnu11 -Wall -Wno-unused-function -ffp-contract=off)
target_link_libraries(fall PRIVATE porthost)

# The virtual PC on its own, driven through the entry points XnGine uses, with the game's
# own image, palette and sounds (port/test/vpcdemo.c)
add_executable(vpcdemo ${CMAKE_CURRENT_SOURCE_DIR}/test/vpcdemo.c)
target_compile_options(vpcdemo PRIVATE -std=gnu11 -Wall)
target_link_libraries(vpcdemo PRIVATE porthost)
