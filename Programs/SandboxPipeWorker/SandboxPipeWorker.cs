using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject;

namespace SandboxPipeWorker;

public static class Sandbox
{
    public static DirectoryReference RootDirectory
    {
        get
        {
            var baseDirectory = AppDomain.CurrentDomain.BaseDirectory;
            baseDirectory = Path.GetFullPath(baseDirectory).Replace('\\', '/');
            var rootIndex = baseDirectory.IndexOf("Programs/SandboxPipeWorker", StringComparison.InvariantCultureIgnoreCase);
            if (rootIndex >= 0)
            {
                return new DirectoryReference(baseDirectory.Substring(0, rootIndex));
            }

            throw new Exception("Could not find root directory");
        }
    }

    public static readonly string PrimaryProjectName = "Sandbox";
}

public class SandboxPipeWorker
{
    private static int Main(string[] arguments)
    {
        // var cMake = new CMake();
        // cMake.GenerateProjectFiles(new PlatformProjectGeneratorCollection());
        var rider = new Rider();
        rider.GenerateProjectFiles(new PlatformProjectGeneratorCollection());
        return 0;
    }
}