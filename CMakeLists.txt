cmake_minimum_required(VERSION 3.12)
project(ghost LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    if(EXISTS "/opt/homebrew")
        list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    else()
        list(APPEND CMAKE_PREFIX_PATH "/usr/local")
    endif()
endif()

find_package(SFML 3 REQUIRED COMPONENTS Window Graphics Audio Network)

file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.cpp")

add_executable(ghost main.cpp ${SOURCES})

target_link_libraries(ghost PRIVATE 
    SFML::Window 
    SFML::Graphics 
    SFML::Audio 
    SFML::Network
)