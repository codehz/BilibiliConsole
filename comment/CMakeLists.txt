find_package(PugiXML REQUIRED)
add_library(comment comment.cpp)
include_directories(PUGIXML_INCLUDE_DIR)
add_library(comment_parser comment_parser.cpp)
