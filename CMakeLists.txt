cmake_minimum_required (VERSION  3.0.0)
project (BilibiliConsole, VERSION 0.0.1)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory(player)
add_subdirectory(test)
add_subdirectory(comment)
