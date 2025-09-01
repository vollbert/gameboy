set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(gameboyEmulator VERSION 0.1.0)

set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(glfw3 CONFIG REQUIRED)
message(STATUS "GLFW found: ${glfw3_LIBRARIES} (includes: ${glfw3_INCLUDE_DIRS})")
find_package(OpenGL REQUIRED)

add_library(glad STATIC ${PROJECT_SOURCE_DIR}/src/glad/src/glad.c)

add_library(font STATIC 
  ${PROJECT_SOURCE_DIR}/src/font/reader.cpp
  ${PROJECT_SOURCE_DIR}/src/font/TTFReader.cpp
)


add_executable(${PROJECT_NAME} 
                src/main.cpp
              )

target_link_libraries(${PROJECT_NAME} PRIVATE glfw OpenGL glad font)

target_include_directories(glad PUBLIC
    ${PROJECT_SOURCE_DIR}/src/glad/include # This makes 'include/' discoverable for glad/glad.h
)


