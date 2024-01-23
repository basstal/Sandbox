# {CompilableModuleTypes: [Cpp], PrimaryProjectName: "Sandbox", PrimaryCompileEnvironment: SandboxPipeWorker.GenerateProject.CppProject.CompileEnvironment, PreprocessorDefinitions: [], AllDependencies: [SandboxPipeWorker.GenerateProject.CppProject.Module, SandboxPipeWorker.GenerateProject.CppProject.Module, SandboxPipeWorker.GenerateProject.CppProject.Module, SandboxPipeWorker.GenerateProject.CppProject.Module, SandboxPipeWorker.GenerateProject.CppProject.Module, SandboxPipeWorker.GenerateProject.CppProject.Module, SandboxPipeWorker.GenerateProject.CppProject.Module]}

cmake_minimum_required(VERSION 3.10)


project(Sandbox)

set(BUILD_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/BuildProject")
set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/Output)
# 设置运行时、库和归档输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_executable(Sandbox ${CMAKE_SOURCE_DIR}/Source/Main.cpp )

add_subdirectory(${CMAKE_SOURCE_DIR}/Source/Infrastructures)
add_subdirectory(${CMAKE_SOURCE_DIR}/Source/Rendering)
add_subdirectory(${CMAKE_SOURCE_DIR}/Plugins/glfw-3.3.9.bin.WIN64)
add_subdirectory(${CMAKE_SOURCE_DIR}/Plugins/glm-0.9.9.8)
add_subdirectory(${CMAKE_SOURCE_DIR}/Plugins/stb_image)
add_subdirectory(${CMAKE_SOURCE_DIR}/Plugins/assimp-5.2.5)
add_subdirectory(${CMAKE_SOURCE_DIR}/Plugins/vulkan-1.3.268)

target_link_libraries(Sandbox PRIVATE Infrastructures Rendering assimp glm vulkan glfw) # TODO:移除固定的导入

target_include_directories(Sandbox PRIVATE ${CMAKE_SOURCE_DIR}/Source/Infrastructures ${CMAKE_SOURCE_DIR}/Source/Rendering )


# copy assets
add_custom_target(
    CopyAssets ALL
    COMMAND PowerShell -ExecutionPolicy Bypass -File CopyFiles.ps1 -InputPath "${CMAKE_SOURCE_DIR}/Assets" -OutputTo "${OUTPUT_DIR}/Assets"
    WORKING_DIRECTORY ${BUILD_PROJECT_DIR}
    COMMENT "Running PowerShell script CopyFiles to copy assets"
)

# copy binaries and compile shaders
add_custom_command(
    TARGET Sandbox POST_BUILD
    COMMAND PowerShell -ExecutionPolicy Bypass -File CopyFiles.ps1 -InputPath "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" -OutputTo "${OUTPUT_DIR}/Binaries" -Flat -Clean
    COMMAND PowerShell -ExecutionPolicy Bypass -File CompileAllShaders.ps1 -GlslcPath "${Vulkan_INCLUDE_DIRS}/../Bin/glslc" -OutputFilesTo "${OUTPUT_DIR}/Binaries/Shaders" -ShadersPath "${CMAKE_SOURCE_DIR}/Source/Shaders"
    WORKING_DIRECTORY ${BUILD_PROJECT_DIR}
    COMMENT "Running PowerShell script CopyFiles to copy binaries & CompileAllShaders"
)







