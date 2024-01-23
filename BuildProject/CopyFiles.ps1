# 复制目录下所有文件到一个给定命令行参数的目录中

param(
    [string]$InputPath,
    [string]$OutputTo,
    [switch]$Flat,
    [switch]$Clean
)

# 创建目标目录，如果它还不存在的话
if (-not (Test-Path $OutputTo)) {
    New-Item -ItemType Directory -Path $OutputTo
} else {
    if ($Clean) {
        # 清空目标目录
        Remove-Item -Path $OutputTo -Recurse -Force
        New-Item -ItemType Directory -Path $OutputTo
    }
}


# 获取源目录中的所有文件
$Files = Get-ChildItem -Path $InputPath -Recurse | Where-Object {!$_.PSIsContainer}

foreach ($File in $Files) {
    if ($Flat) {
        # 在平坦模式下，目标路径是目标根目录
        $TargetPath = Join-Path $OutputTo $File.Name

        # 检查目标路径上是否已存在文件
        if (Test-Path $TargetPath) {
            Write-Warning "文件复制失败，目标位置已存在文件：'$TargetPath'"
        } else {
            # 复制文件到目标路径
            Copy-Item -Path $File.FullName -Destination $TargetPath
        }
    } else {
        # 目标路径是在目标根目录下，保持相对于源目录的相同路径
        $TargetPath = Join-Path $OutputTo $File.FullName.Substring($InputPath.Length)

        # 确保目标路径的文件夹存在
        $TargetDir = Split-Path $TargetPath
        if (-not (Test-Path $TargetDir)) {
            New-Item -ItemType Directory -Path $TargetDir
        }

        # 复制文件到目标路径
        Copy-Item -Path $File.FullName -Destination $TargetPath -Force
    }
}