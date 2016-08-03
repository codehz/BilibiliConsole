find_package(SDL2 REQUIRED)
find_package(LIBVLC REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} $(LIBVLC_INCLUDE_DIR))
add_library(player player.cpp)
