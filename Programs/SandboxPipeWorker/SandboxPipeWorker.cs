using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject;

namespace SandboxPipeWorker;

public static class Sandbox
{
    private static readonly string TempRootDirectoryStr = "C:/Users/xj/Documents/GitHub/Sandbox";
    public static readonly DirectoryReference RootDirectory = new DirectoryReference(TempRootDirectoryStr);
    public static readonly string PrimaryProjectName = "Sandbox";
}

public class SandboxPipeWorker
{
    private static int Main(string[] arguments)
    {
        var cMake = new CMake();
        cMake.GenerateProjectFiles(new PlatformProjectGeneratorCollection());
        return 0;
    }
}