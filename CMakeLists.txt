cmake_minimum_required(VERSION 3.12)

#Project name
project(PBR Renderer)

#C++ minimum version
set(CMAKE_CXX_STANDARD 17)

#add debug 
add_definitions (-DDEBUG)

#add assets
file(GLOB ASSETS "./assets" "./thirdParty/bin/assimp/assimp-vc143-mtd.dll")

file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

#include third party 
include_directories(

	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/include
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/application/miniz)

link_directories(

	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/thirdParty/lib
)

#add wrapper subdirectory
add_subdirectory(wrapper)
add_subdirectory(application)
add_subdirectory(glframework)
add_subdirectory(imgui)

#Main program file and name
add_executable(pbr "main.cpp" "glad.c")

#add glfw3 lib to exe
target_link_libraries(pbr glfw3.lib assimp-vc143-mtd.lib zlibstaticd.lib wrapper app fw imguilib)