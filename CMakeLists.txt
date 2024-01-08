cmake_minimum_required(VERSION 3.10)

project(Sandbox)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture -Wno-unused-local-typedef -Wno-unused-private-field -Wno-unused-const-variable -Wno-unused-lambda-capture")

find_package(Vulkan REQUIRED)

# 将 Source 目录下的源码纳入项目中

file(GLOB_RECURSE SOURCE_FILES
    Source/*.cpp
    Source/*.h
    Source/*.hpp
    Source/*.c
    Source/*.cc
    Source/*.hh
    Source/*.inl
    Source/*.inc
    Source/*.txt
    Source/*.md
    Source/*.glsl
    Source/*.vert
    Source/*.frag
    Source/*.geom
    Source/*.comp
    Source/*.tesc
    Source/*.tese
    Source/*.rgen
    Source/*.rchit
    Source/*.rmiss
    Source/*.rcall
    Source/*.mesh
    Source/*.task
    Source/*.rgen
    Source/*.rchit
    Source/*.rmiss
    Source/*.rcall
    Source/*.mesh
    Source/*.task
    Source/*.rgen
    Source/*.rchit
    Source/*.rmiss
    Source/*.rcall
    Source/*.mesh
    Source/*.task
    Source/*.rgen
    Source/*.rchit
    Source/*.rmiss
    Source/*.rcall
    Source/*.mesh
    Source/*.task
    Source/*.rgen
    Source/*.rchit
    Source/*.rmiss
    Source/*.rcall
    Source/*.mesh
    Source/*.task
    Source/*.rgen
    Source/*.rchit
    Source/*.rmiss
    Source/*.rcall
    Source/*.mesh
    Source/*.task
    Source/*.rgen
    Source/*.rchit
    Source/*.rmiss
    Source/*.rcall
    Source/*.mesh
    Source/*.task
    Source/*.rgen
    Source/*.rchit
    Source/*.rmiss
    Source/*.rcall
)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/glfw-3.3.9.bin.WIN64/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/glm-0.9.9.8/glm
)

target_link_libraries(
    ${PROJECT_NAME}
    ${Vulkan_LIBRARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/glfw-3.3.9.bin.WIN64/lib-vc2022/glfw3.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/glm-0.9.9.8/MinSizeRel/glm_static.lib
)

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.LIB")
endif()