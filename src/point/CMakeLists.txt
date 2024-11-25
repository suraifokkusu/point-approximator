add_library(point PointManager.cpp)
target_include_directories(point PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(point PRIVATE sfml-graphics)

# Копирование файла arial.ttf в папку библиотеки point
add_custom_command(TARGET point POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/arial.ttf
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )