
try {
    & "C:/Users/xj/Documents/GitHub/Sandbox/Sources/../BuildProject/CopyFiles.ps1" -InputFile "C:/Users/xj/Documents/GitHub/Sandbox/Plugins/assimp-5.2.5/bin/x64/assimp-vc143-mt.dll" -OutputTo "C:/Users/xj/Documents/GitHub/Sandbox/Output" -Flat -Force
} catch {
    Write-Error "发生错误: $_"
    exit 1 
}

try {
    & "C:/Users/xj/Documents/GitHub/Sandbox/Sources/../BuildProject/CompileAllShaders.ps1" -GlslcPath "C:\VulkanSDK\1.3.268.0/Bin/glslc" -OutputFilesTo "C:/Users/xj/Documents/GitHub/Sandbox/Output/Shaders" -ShadersPath "C:/Users/xj/Documents/GitHub/Sandbox/Sources/Shaders"
} catch {
    Write-Error "发生错误: $_"
    exit 2
}