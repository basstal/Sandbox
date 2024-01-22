namespace SandboxPipeWorker.Common;

public class DirectoryReference : FileSystemBase, IEquatable<DirectoryReference>, IComparable<DirectoryReference>
{
    public int CompareTo(DirectoryReference? other)
    {
        return Comparer.Compare(FullName, other?.FullName);
    }

    public static bool operator ==(DirectoryReference? left, DirectoryReference? right)
    {
        if (left is null)
        {
            return right is null;
        }

        if (right is null)
        {
            return false;
        }

        return left.FullName.Equals(right.FullName, Comparison);
    }

    public static bool operator !=(DirectoryReference? left, DirectoryReference? right)
    {
        return !(left == right);
    }

    public override bool Equals(object? obj)
    {
        return obj is DirectoryReference directory && directory == this;
    }

    public override int GetHashCode()
    {
        return Comparer.GetHashCode(FullName);
    }

    public bool Equals(DirectoryReference? obj)
    {
        return obj == this;
    }

    public DirectoryReference(string fullName) : base(fullName)
    {
    }

    public DirectoryReference Combine(params string[] fragments)
    {
        string combinedPath = FullName;
        foreach (string fragment in fragments)
        {
            combinedPath = Path.Combine(combinedPath, fragment);
            if (!Directory.Exists(combinedPath))
            {
                throw new Exception($"Directory not exists: {combinedPath}");
            }
        }

        return new DirectoryReference(combinedPath);
    }

    public IEnumerable<DirectoryReference> EnumerateDirectories()
    {
        return Directory.EnumerateDirectories(FullName).Select(path => new DirectoryReference(path));
    }

    public IEnumerable<FileReference> EnumerateFiles()
    {
        return Directory.EnumerateFiles(FullName).Select(path => new FileReference(path));
    }

    public string GetDirectoryName()
    {
        return Path.GetFileName(FullName);
    }

    public FileReference GetFile(string fileName)
    {
        return new FileReference(Path.Combine(FullName, fileName));
    }
}