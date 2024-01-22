namespace SandboxPipeWorker.Common;

public abstract class FileSystemBase
{
    protected FileSystemBase(string fullName)
    {
        FullName = fullName;
    }

    public string FullName { get; }
    public static readonly StringComparer Comparer = StringComparer.OrdinalIgnoreCase;
    public static readonly StringComparison Comparison = StringComparison.OrdinalIgnoreCase;
}