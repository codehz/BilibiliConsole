find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(LIBVLC REQUIRED)

add_executable(test_player test_player.cpp)
add_executable(test_comment test_comment.cpp)
add_custom_command(TARGET test_player PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/test/res $<TARGET_FILE_DIR:test_player>/res)
target_link_libraries(test_player player ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${LIBVLC_LIBRARY})
target_link_libraries(test_comment comment)
