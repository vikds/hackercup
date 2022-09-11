cmake_minimum_required(VERSION 3.0)

project (hackercup)

set(SOURCES
    src/main.cpp
)

add_executable(hackercup ${SOURCES})

target_include_directories(hackercup
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)

target_compile_features(${PROJECT_NAME}
    PRIVATE cxx_std_17
)