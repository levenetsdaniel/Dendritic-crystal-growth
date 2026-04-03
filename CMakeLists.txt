cmake_minimum_required(VERSION 3.5)
project(crystal)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

find_package(SFML REQUIRED COMPONENTS Graphics Window System)

set(SRC src/main.cpp src/Grid.cpp src/Field.cpp src/PhaseField.cpp src/TemperatureField.cpp
        src/MathFunctions.cpp include/FieldRender.hpp)

add_executable(crystal ${SRC})

target_include_directories(crystal PRIVATE
        include
        /opt/homebrew/Cellar/sfml/3.0.2/include
)

target_link_libraries(crystal
        SFML::Graphics
        SFML::Window
        SFML::System
)
