cmake_minimum_required(VERSION 2.7)

project (hackercup)

set(SOURCES
    src/main.cpp
)

add_executable(hackercup ${SOURCES})

target_include_directories(hackercup
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)