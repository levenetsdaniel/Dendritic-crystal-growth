cmake_minimum_required(VERSION 3.16)
project(crystal CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

file(GLOB SOURCES "src/*.cpp")

add_executable(crystal ${SOURCES})

target_include_directories(crystal PRIVATE include)

target_link_libraries(crystal PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)

if(UNIX AND NOT APPLE)
    find_package(OpenMP REQUIRED)
    target_link_libraries(crystal PRIVATE OpenMP::OpenMP_CXX)
endif()

if(APPLE)
    set(OpenMP_INCLUDE_DIR "/opt/homebrew/opt/libomp/include")
    set(OpenMP_LIB "/opt/homebrew/opt/libomp/lib/libomp.dylib")
    target_compile_options(crystal PRIVATE -Xpreprocessor -fopenmp)
    target_include_directories(crystal PRIVATE ${OpenMP_INCLUDE_DIR})
    target_link_libraries(crystal PRIVATE ${OpenMP_LIB})
endif()