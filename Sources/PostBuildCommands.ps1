
try {
    & "E:/Documents/git-repository/Sandbox/Sources/../BuildProject/CopyFiles.ps1" -InputFile "E:/Documents/git-repository/Sandbox/Plugins/assimp-5.2.5/bin/x64/assimp-vc143-mt.dll" -OutputTo "E:/Documents/git-repository/Sandbox/Output" -Flat -Force
} catch {
    Write-Error "发生错误: $_"
    exit 1 
}

try {
    & "E:/Documents/git-repository/Sandbox/Sources/../BuildProject/CompileAllShaders.ps1" -GlslcPath "C:\VulkanSDK\1.3.268.0/Bin/glslc" -OutputFilesTo "E:/Documents/git-repository/Sandbox/Output/Shaders" -ShadersPath "E:/Documents/git-repository/Sandbox/Sources/Shaders"
} catch {
    Write-Error "发生错误: $_"
    exit 2
}