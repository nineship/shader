cmake_minimum_required(VERSION 3.16)
project(show)


set(CMAKE_CXX_STANDARD 11)


set(CMAKE_EXE_LINKER_FLAGS "-static")
#SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lglfw3 -lpthread")


include_directories("./glfw/include")
include_directories("./glm")
include_directories("./learn")
include_directories("./glad/include")
link_directories("./glfw/lib-mingw-w64")
include_directories("./learnopengl")
set(
SOURCE_DIR main.cpp glad.c stb_image.cpp version.rc
)
link_libraries("libglfw3.a")
link_libraries("libglfw3dll.a")

add_executable(show ${SOURCE_DIR})


target_link_libraries(show libglfw3.a)
target_link_libraries(show libglfw3dll.a)
target_link_libraries(show libgcc.a)
target_link_libraries(show libstdc++.a)
target_link_libraries(show libpthread.a )