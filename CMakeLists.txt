cmake_minimum_required(VERSION 3.17.5)
project(PointApproximator LANGUAGES CXX)

# Установка стандартов
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Выходная директория для сборки
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Опция для статической сборки библиотек
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Подключение SFML через FetchContent
include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(SFML)

# Добавление модулей
add_subdirectory(src/app)
add_subdirectory(src/grid)
add_subdirectory(src/point)

# Основной исполняемый файл
add_executable(main src/main.cpp)

# Линковка SFML и модулей
target_link_libraries(main PRIVATE sfml-graphics sfml-window sfml-system app grid point)