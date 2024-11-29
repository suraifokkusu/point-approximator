add_library(lavalamp LavaLamp.cpp)
target_include_directories(lavalamp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lavalamp PRIVATE sfml-graphics)