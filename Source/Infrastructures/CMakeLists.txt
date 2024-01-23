# {Name: "Infrastructures", Type: Cpp, ParsedFile: SandboxPipeWorker.Common.FileReference, CompileEnvironment: SandboxPipeWorker.GenerateProject.CppProject.CompileEnvironment, PrecompileEnvironment: null, RegisteredModules: [{Key: "Infrastructures", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "Rendering", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glfw", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glm", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "stb", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "assimp", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "vulkan", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}], SourceDirectory: SandboxPipeWorker.Common.DirectoryReference, RelativeModuleSourcePath: ["NativeFileSystem.cpp"]}

cmake_minimum_required(VERSION 3.10)


add_library(Infrastructures SHARED NativeFileSystem.cpp )

target_link_libraries(Infrastructures PRIVATE )

target_include_directories(Infrastructures PRIVATE )

# 为共享库目标添加编译定义
target_compile_definitions(Infrastructures PRIVATE INFRASTRUCTURES_EXPORT_API)







