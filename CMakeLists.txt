cmake_minimum_required(VERSION 3.13)
project(ps3_update_bruteforce C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS_INIT "-Wall -Wextra -std=c11")
SET(CMAKE_C_FLAGS_DEBUG_INIT "-Ofast")

find_package(CURL)
if (NOT CURL_FOUND)
    MESSAGE(FATAL_ERROR "Could not find the CURL library and include files.")
endif ()

add_executable(ps3_update_bruteforce main.c)
target_link_libraries(ps3_update_bruteforce ${CURL_LIBRARIES})
target_include_directories(ps3_update_bruteforce PRIVATE ${CURL_INCLUDE_DIRS})