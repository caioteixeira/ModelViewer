cmake_minimum_required (VERSION 3.8.2)
project (ModelViewer)

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG=1 /MTd /Od /Zi")
set(CMAKE_CXX_FLAGS_RELEASE "-DAK_OPTIMIZED=1 /MT")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG=1 /MTd /Od /Zi")
set(CMAKE_C_FLAGS_RELEASE "/MT")

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(External/bgfx.cmake EXCLUDE_FROM_ALL)
add_subdirectory(External/glfw EXCLUDE_FROM_ALL)
add_subdirectory(External/portable-file-dialogs EXCLUDE_FROM_ALL)

add_subdirectory(External/tinyobjloader EXCLUDE_FROM_ALL)

add_custom_target(
	build-shaders
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shaders
	COMMAND $<TARGET_FILE:shaderc> --platform windows -p vs_5_0 -f ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vs_phong.sc 
	-o ${CMAKE_CURRENT_BINARY_DIR}/shaders/vs_phong.bin --type v -i ${BGFX_DIR}/src --varyingdef ${CMAKE_CURRENT_SOURCE_DIR}/shaders/varying.def.sc
	COMMAND $<TARGET_FILE:shaderc> --platform windows -p ps_5_0 -f ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fs_phong.sc 
	-o ${CMAKE_CURRENT_BINARY_DIR}/shaders/fs_phong.bin --type f -i ${BGFX_DIR}/src --varyingdef ${CMAKE_CURRENT_SOURCE_DIR}/shaders/varying.def.sc
)
add_dependencies(build-shaders shaderc)

add_executable(ModelViewer  src/main.cpp src/graphics.h src/graphics.cpp src/modelLoader.h src/modelLoader.cpp)
add_dependencies(ModelViewer build-shaders)

set_target_properties(ModelViewer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BUILD_DIR}")

target_link_libraries(ModelViewer bgfx bx bimg glfw portable_file_dialogs tinyobjloader)
