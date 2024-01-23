# {Name: "glm", Type: StaticLibrary, ParsedFile: SandboxPipeWorker.Common.FileReference, CompileEnvironment: null, PrecompileEnvironment: SandboxPipeWorker.GenerateProject.CppProject.PrecompileEnvironment, RegisteredModules: [{Key: "Infrastructures", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "Rendering", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glfw", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glm", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "stb", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "assimp", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "vulkan", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}], SourceDirectory: SandboxPipeWorker.Common.DirectoryReference, RelativeModuleSourcePath: null}

cmake_minimum_required(VERSION 3.10)


add_library(glm INTERFACE)

# 链接到预编译的库和包含头文件的目录
target_link_libraries(glm INTERFACE ${CMAKE_SOURCE_DIR}/Plugins/glm-0.9.9.8/MinSizeRel/glm_static.lib )
target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/Plugins/glm-0.9.9.8/glm )







