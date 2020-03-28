project(7DRL)

cmake_minimum_required(VERSION 3.3)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-g")

add_compile_definitions(WITH_ALSA)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(./libtcod/src/)
include_directories(./src/soloud/include/)

file(GLOB_RECURSE U101_SOURCES "src/*.cpp" "src/*.c")

link_directories(./)

add_library(libtcod SHARED IMPORTED)
set_property(TARGET libtcod PROPERTY IMPORTED_LOCATION "./libtcod.so")
find_package (Threads)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(ALSA REQUIRED)
include_directories(${ALSA_INCLUDE_DIR})

add_executable(7DRL ${U101_SOURCES})

target_link_libraries (7DRL ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (7DRL ${SDL2_LIBRARIES})
target_link_libraries (7DRL libtcod)
target_link_libraries (7DRL dl)
target_link_libraries (7DRL ${ALSA_LIBRARY})
