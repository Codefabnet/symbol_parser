# Create build system: cmake -B build
# Build command:       cmake --build build (binaries in build/output)
# Clean command:       make -C build clean

cmake_minimum_required(VERSION 3.10)

project(funcs)

set(SRCDIR "./source")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/output)

set(SOURCES
        ${SRCDIR}/funcs.c
        ${SRCDIR}/symbol_definition_functions.c
        ${SRCDIR}/funcs_parse_functions.c
        ${SRCDIR}/vars_parse_functions.c
        ${SRCDIR}/common_parse_functions.c)

add_executable(funcs ${SOURCES})

add_executable(vars ${SOURCES})

