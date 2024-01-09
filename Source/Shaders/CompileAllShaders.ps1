# 接受命令行参数或使用默认值
param(
    [string]$GlslcPath = "glslc",
    [string]$OutputFilesTo = "../../build/Debug/shaders"
)

# 如果目标目录不存在则创建目标目录文件夹
if (-not (Test-Path $OutputFilesTo)) {
    New-Item -ItemType Directory -Force -Path $OutputFilesTo
}

# 获取当前目录下所有 .vert 和 .frag 文件
$ShaderFiles = Get-ChildItem -Path .\* -Include *.vert, *.frag

foreach ($File in $ShaderFiles) {
    # 构建输出文件名（将 .vert 替换为 _vert.spv，将 .frag 替换为 _frag.spv）
    $OutputFileName = $File.Name -replace "\.vert$", "_vert.spv" -replace "\.frag$", "_frag.spv"

    $OutputPath = Join-Path $OutputFilesTo $OutputFileName
    # 构建和执行 glslc 命令
    $GlslcCommand = "$GlslcPath `"$($File.FullName)`" -o `"$OutputPath`""
    Invoke-Expression $GlslcCommand

    Write-Host "Compiled: $($File.Name) -> $OutputFileName"
}
