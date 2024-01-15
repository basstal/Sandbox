cmake_minimum_required(VERSION 3.10)

project(Sandbox)

set(CMAKE_CXX_STANDARD 17)

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders")
set(BUILD_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/BuildProject")
set(OUTPUT_DIR ${CMAKE_BINARY_DIR})

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

# compile shaders
add_custom_target(
    CompileAllShaders ALL
    COMMAND PowerShell -ExecutionPolicy Bypass -File CompileAllShaders.ps1 -GlslcPath "${Vulkan_INCLUDE_DIRS}/../Bin/glslc" -OutputFilesTo "${OUTPUT_DIR}" -ShadersPath "${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders"
    WORKING_DIRECTORY ${BUILD_PROJECT_DIR}
    COMMENT "Running PowerShell script CompileAllShaders"
)

# copy asssets
add_custom_target(
    CopyAssets ALL
    COMMAND PowerShell -ExecutionPolicy Bypass -File CopyAssets.ps1 -AssetsPath "${CMAKE_CURRENT_SOURCE_DIR}/Assets" -OutputFilesTo "${OUTPUT_DIR}"
    WORKING_DIRECTORY ${BUILD_PROJECT_DIR}
    COMMENT "Running PowerShell script CopyAssets"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/glfw-3.3.9.bin.WIN64/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/glm-0.9.9.8/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/assimp-5.2.5/include

)

target_link_libraries(
    ${PROJECT_NAME}
    ${Vulkan_LIBRARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/glfw-3.3.9.bin.WIN64/lib-vc2022/glfw3.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/glm-0.9.9.8/MinSizeRel/glm_static.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Plugins/assimp-5.2.5/lib/x64/assimp-vc143-mt.lib
)

target_compile_definitions(
    ${PROJECT_NAME}
    PUBLIC
    UNICODE
)

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT.LIB")
endif()