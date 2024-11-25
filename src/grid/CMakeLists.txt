add_library(grid Grid.cpp)
target_include_directories(grid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(grid PRIVATE sfml-graphics)