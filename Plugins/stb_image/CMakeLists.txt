# {Name: "stb", Type: HeaderOnlyLibrary, ParsedFile: SandboxPipeWorker.Common.FileReference, CompileEnvironment: null, PrecompileEnvironment: SandboxPipeWorker.GenerateProject.CppProject.PrecompileEnvironment, RegisteredModules: [{Key: "Infrastructures", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "Rendering", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glfw", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glm", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "stb", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "assimp", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "vulkan", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}], SourceDirectory: SandboxPipeWorker.Common.DirectoryReference, RelativeModuleSourcePath: null}

cmake_minimum_required(VERSION 3.10)


add_library(stb INTERFACE)

target_include_directories(stb INTERFACE ${CMAKE_SOURCE_DIR}/Plugins/stb_image/stb )







