using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class PrecompileEnvironment
{
    public List<DirectoryReference> IncludePaths = new List<DirectoryReference>();
    public List<FileReference> DllPaths = new List<FileReference>();
    public List<FileReference> LibPaths = new List<FileReference>();
}