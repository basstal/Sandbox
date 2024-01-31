using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class PrecompileEnvironment
{
    public List<FileSystemBase> IncludePaths = new List<FileSystemBase>();
    public List<FileReference> DllPaths = new List<FileReference>();
    public List<FileReference> LibPaths = new List<FileReference>();
}