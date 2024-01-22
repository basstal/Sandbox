# {CompilableModuleTypes: [Cpp], PrimaryProjectName: "Sandbox", PrimaryCompileEnvironment: SandboxPipeWorker.GenerateProject.CppProject.CompileEnvironment, PreprocessorDefinitions: []}

cmake_minimum_required(VERSION 3.10)




project(Sandbox)

add_executable(Sandbox Source/Main.cpp )

add_subdirectory(Source/Rendering )


set(CMAKE_CXX_STANDARD Cpp20)

find_package(Vulkan REQUIRED)

target_include_directories(Sandbox PRIVATE Source/Rendering)

target_link_libraries(Sandbox PRIVATE Rendering )


