cmake_minimum_required(VERSION 3.5)
project(crystal)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(SRC src/Grid.cpp src/Field.cpp src/PhaseField.cpp src/TemperatureField.cpp
        src/MathFunctions.cpp)

add_executable(main ${SRC})

add_executable(crystal main.cpp)
