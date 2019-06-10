cmake_minimum_required (VERSION 3.8.2)
project (ModelViewer)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG=1 /MTd /Od")
set(CMAKE_CXX_FLAGS_RELEASE "-DAK_OPTIMIZED=1 /MT")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG=1 /MTd /Od")
set(CMAKE_C_FLAGS_RELEASE "/MT")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(External/bgfx.cmake EXCLUDE_FROM_ALL)
add_subdirectory(External/glfw EXCLUDE_FROM_ALL)
add_subdirectory(External/portable-file-dialogs EXCLUDE_FROM_ALL)

add_executable(ModelViewer  src/main.cpp src/graphics.h src/graphics.cpp)

target_link_libraries(ModelViewer bgfx bx bimg glfw portable_file_dialogs)
