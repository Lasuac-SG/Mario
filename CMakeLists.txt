cmake_minimum_required(VERSION 3.16)

project(Mario)

find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)

add_subdirectory(src)
