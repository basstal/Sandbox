using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject;

namespace SandboxPipeWorker;

public static class Sandbox
{
    private static DirectoryReference? _RootDirectory;
    public static DirectoryReference RootDirectory
    {
        get
        {
            if (_RootDirectory == null)
            {
                var baseDirectory = AppDomain.CurrentDomain.BaseDirectory;
                baseDirectory = Path.GetFullPath(baseDirectory).Replace('\\', '/');
                var rootIndex = baseDirectory.IndexOf("Programs/SandboxPipeWorker", StringComparison.InvariantCultureIgnoreCase);
                if (rootIndex < 0)
                {
                    throw new Exception("Could not find root directory");
                }
                _RootDirectory = new DirectoryReference(baseDirectory.Substring(0, rootIndex));
            }
            return _RootDirectory;
        }
    }

    public static DirectoryReference SourceDirectory
    {
        get
        {
            return Sandbox.RootDirectory.GetDirectory("Programs/SandboxPipeWorker");
        }
    }

    public static readonly string PrimaryProjectName = "Sandbox";
}

public class SandboxPipeWorker
{
    private static int Main(string[] arguments)
    {
        // Console.WriteLine(Directory.GetCurrentDirectory());
        // Console.WriteLine(Sandbox.RootDirectory.FullName);
        // var cMake = new CMake();
        // cMake.GenerateProjectFiles(new PlatformProjectGeneratorCollection());
        var rider = new Rider();
        rider.GenerateProjectFiles(new PlatformProjectGeneratorCollection());
        return 0;
    }
}
