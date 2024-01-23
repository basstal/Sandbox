namespace SandboxPipeWorker.Common;

public abstract class FileSystemBase
{
    protected FileSystemBase(string fullName)
    {
        if (Path.IsPathRooted(fullName))
        {
            FullName = fullName.Replace('\\', '/');
        }
        else
        {
            FullName = Path.Combine(Sandbox.RootDirectory.FullName, fullName).Replace('\\', '/');
        }
    }

    public string RelativeEnginePath => GetRelativePath(Sandbox.RootDirectory.FullName);

    public string FullName { get; }
    public static readonly StringComparer Comparer = StringComparer.OrdinalIgnoreCase;
    public static readonly StringComparison Comparison = StringComparison.OrdinalIgnoreCase;

    public string GetRelativePath(string relativeTo)
    {
        return Path.GetRelativePath(relativeTo, FullName).Replace('\\', '/');
    }
}