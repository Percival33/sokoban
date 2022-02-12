cmake_minimum_required(VERSION 3.21)
project(sokoban)

set(CMAKE_CXX_STANDARD 11)

set(SDL2_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/SDL2-2.0.10/include)
set(SDL2_LIB_DIR ${PROJECT_SOURCE_DIR}/SDL2-2.0.10/lib/x64)

include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIB_DIR})

set(SOURCE_FILES main.cpp)
add_executable(sokoban main.cpp draw.cpp draw.h consts.h game.cpp game.h graphics.h colors.h player.h board.h chest.h)

target_link_libraries(${PROJECT_NAME} SDL2main SDL2)