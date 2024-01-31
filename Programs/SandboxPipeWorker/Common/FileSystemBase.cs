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

    public abstract FileReference[] GetFiles(string pattern, bool recursive = true, bool useRegex = false);

    public static FileSystemBase Create(string path, string? relativeTo = null)
    {
        var combinedPath = path;
        if (!Path.IsPathRooted(path))
        {
            relativeTo ??= Sandbox.RootDirectory.FullName;
            combinedPath = Path.Combine(relativeTo, path);
        }

        var file = new FileReference(combinedPath);
        if (file.Exists())
        {
            return file;
        }

        var directory = new DirectoryReference(combinedPath);
        if (directory.Exists())
        {
            return directory;
        }

        throw new Exception($"{combinedPath} don't exist.");
    }

    public abstract bool Exists();
}