namespace SandboxPipeWorker.Common;

public class FileReference : FileSystemBase, IEquatable<FileReference>, IComparable<FileReference>
{
    internal static readonly List<FileReference> EmptyList = new List<FileReference>(0);

    public FileReference(string fullName) : base(fullName)
    {
    }

    public int CompareTo(FileReference? other)
    {
        return Comparer.Compare(FullName, other?.FullName);
    }

    public static bool operator ==(FileReference? left, FileReference? right)
    {
        if (left is null)
        {
            return right is null;
        }

        if (right is null)
        {
            return false;
        }

        return Path.GetFullPath(left.FullName).Equals(Path.GetFullPath(right.FullName), Comparison);
    }

    public static bool operator !=(FileReference? left, FileReference? right)
    {
        return !(left == right);
    }

    public override bool Equals(object? obj)
    {
        return obj is FileReference file && file == this;
    }

    public override int GetHashCode()
    {
        return Comparer.GetHashCode(FullName);
    }

    public bool Equals(FileReference? obj)
    {
        return obj == this;
    }

    public bool HasExtension(string extension)
    {
        return FullName.EndsWith(extension, Comparison);
    }

    public string GetFileNameWithoutExtension(string extension)
    {
        if (!string.IsNullOrEmpty(extension) && HasExtension(extension))
        {
            return Path.GetFileName(FullName.Substring(0, FullName.Length - extension.Length));
        }

        return Path.GetFileNameWithoutExtension(FullName);
    }

    public DirectoryReference GetDirectory()
    {
        return new DirectoryReference(Path.GetDirectoryName(FullName)!);
    }

    public override FileReference[] GetFiles(string pattern, bool recursive = true, bool useRegex = false)
    {
        // TODO: pattern match

        return new[]
        {
            this
        };
    }

    public override bool Exists()
    {
        return File.Exists(FullName);
    }

    public string ReadAllText()
    {
        return File.ReadAllText(FullName);
    }
}