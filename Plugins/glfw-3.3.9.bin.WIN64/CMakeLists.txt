# {Name: "glfw", Type: StaticLibrary, ParsedFile: SandboxPipeWorker.Common.FileReference, CompileEnvironment: null, PrecompileEnvironment: SandboxPipeWorker.GenerateProject.CppProject.PrecompileEnvironment, RegisteredModules: [{Key: "Infrastructures", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "Rendering", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glfw", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glm", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "stb", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "assimp", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "vulkan", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}], SourceDirectory: SandboxPipeWorker.Common.DirectoryReference, RelativeModuleSourcePath: null}

cmake_minimum_required(VERSION 3.10)


add_library(glfw INTERFACE)

# 链接到预编译的库和包含头文件的目录
target_link_libraries(glfw INTERFACE ${CMAKE_SOURCE_DIR}/Plugins/glfw-3.3.9.bin.WIN64/lib-vc2022/glfw3.lib )
target_include_directories(glfw INTERFACE ${CMAKE_SOURCE_DIR}/Plugins/glfw-3.3.9.bin.WIN64/include )







