add_executable(test_player test_player.cpp)
target_link_libraries(test_player player ${SDL2_LIBRARY} ${LIBVLC_LIBRARY})
