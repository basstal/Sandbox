using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class PrecompileEnvironment
{
    public List<FileReference> IncludePaths = new List<FileReference>();
    public List<DirectoryReference> AdditionalIncludePaths = new List<DirectoryReference>();
    public List<FileReference> DllPaths = new List<FileReference>();
    public List<FileReference> LibPaths = new List<FileReference>();
}