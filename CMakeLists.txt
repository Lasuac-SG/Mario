cmake_minimum_required(VERSION 3.16)

project(Mario)

set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)

add_subdirectory(src)

add_subdirectory(tests)
