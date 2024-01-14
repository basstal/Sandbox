# 复制目录下所有文件到一个给定命令行参数的目录中
# 用法: CopyAssets.ps1 <目标目录>

param(
    [string]$AssetsPath,
    [string]$OutputFilesTo
)

$Destination = Join-Path $OutputFilesTo "Assets"
# 创建目标目录，如果它还不存在的话
if (-not (Test-Path $Destination)) {
    New-Item -ItemType Directory -Path $Destination
}

# 获取源目录中的所有文件和文件夹
$Items = Get-ChildItem -Path $AssetsPath -Recurse

foreach ($Item in $Items) {
    # 目标路径是在目标根目录下，保持相对于源目录的相同路径
    $TargetPath = Join-Path $Destination $Item.FullName.Substring($AssetsPath.Length)

    if ($Item.PSIsContainer) {
        # 如果是文件夹，确保它在目标目录中存在
        if (-not (Test-Path $TargetPath)) {
            New-Item -ItemType Directory -Path $TargetPath
        }
    } else {
        # 复制文件到目标路径
        Copy-Item -Path $Item.FullName -Destination $TargetPath -Force
    }
}
