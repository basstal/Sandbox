# {Name: "Rendering", Type: Cpp, ParsedFile: SandboxPipeWorker.Common.FileReference, CompileEnvironment: SandboxPipeWorker.GenerateProject.CppProject.CompileEnvironment, PrecompileEnvironment: null, RegisteredModules: [{Key: "Rendering", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glfw", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glm", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "stb", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "assimp", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}]}

cmake_minimum_required(VERSION 3.10)




add_library(Rendering SHARED Camera.cpp)


set(CMAKE_CXX_STANDARD Cpp20)

find_package(Vulkan REQUIRED)

target_include_directories(Rendering PRIVATE )

target_link_libraries(Rendering PRIVATE glfw glm stb assimp )


