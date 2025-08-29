cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(gameboyEmulator VERSION 0.1.0)

find_package(glfw3 CONFIG REQUIRED)
message(STATUS "GLFW found: ${glfw3_LIBRARIES} (includes: ${glfw3_INCLUDE_DIRS})")
find_package(OpenGL REQUIRED)


add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL)
