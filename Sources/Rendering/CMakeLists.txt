# {Name: "Rendering", Type: Cpp, ParsedFile: SandboxPipeWorker.Common.FileReference, CompileEnvironment: SandboxPipeWorker.GenerateProject.CppProject.CompileEnvironment, PrecompileEnvironment: null, RegisteredModules: [{Key: "Infrastructures", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "Rendering", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glfw", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "glm", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "stb", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "assimp", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}, {Key: "vulkan", Value: SandboxPipeWorker.GenerateProject.CppProject.Module}], SourceDirectory: SandboxPipeWorker.Common.DirectoryReference, RelativeModuleSourcePath: ["Camera.cpp", "Graphics.cpp", "Image.cpp", "Mesh.cpp", "Model.cpp"]}

cmake_minimum_required(VERSION 3.10)


add_library(Rendering SHARED Camera.cpp Graphics.cpp Image.cpp Mesh.cpp Model.cpp )

target_link_libraries(Rendering PRIVATE glfw glm stb assimp vulkan Infrastructures )

target_include_directories(Rendering PRIVATE ${CMAKE_SOURCE_DIR}/Plugins/glfw-3.3.9.bin.WIN64/include ${CMAKE_SOURCE_DIR}/Plugins/glm-0.9.9.8/glm ${CMAKE_SOURCE_DIR}/Plugins/stb_image/stb ${CMAKE_SOURCE_DIR}/Plugins/assimp-5.2.5/include ${CMAKE_SOURCE_DIR}/Plugins/vulkan-1.3.268/Include ${CMAKE_SOURCE_DIR}/Source/Infrastructures )

# 为共享库目标添加编译定义
target_compile_definitions(Rendering PRIVATE RENDERING_EXPORT_API)







