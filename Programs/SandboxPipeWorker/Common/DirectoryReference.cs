using System.Text.RegularExpressions;

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

    public DirectoryReference GetDirectory(params string[] fragments)
    {
        string combinedPath = FullName;
        foreach (string fragment in fragments)
        {
            combinedPath = Path.Combine(combinedPath, fragment);
            // if (!Directory.Exists(combinedPath))
            // {
            //     throw new Exception($"Directory not exists: {combinedPath}");
            // }
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

    public List<FileReference> SearchFiles(string pattern, bool useRegex = false, bool recursive = true)
    {
        var matchedFiles = new List<FileReference>();
        // 确保提供的目录存在
        if (!Directory.Exists(FullName))
        {
            throw new DirectoryNotFoundException($"The directory '{FullName}' was not found.");
        }

        // 使用通配符搜索
        if (!useRegex)
        {
            try
            {
                matchedFiles.AddRange(Directory.GetFiles(FullName, pattern, recursive
                        ? SearchOption.AllDirectories
                        : SearchOption.TopDirectoryOnly)
                    .Select(file => new FileReference(file)));
            }
            catch (Exception ex)
            {
                Log.Error($"An error occurred: {ex.Message}");
            }
        }
        // 使用正则表达式搜索
        else
        {
            try
            {
                var regex = new Regex(pattern);
                foreach (var file in Directory.GetFiles(FullName, "*", SearchOption.AllDirectories))
                {
                    if (regex.IsMatch(Path.GetFileName(file)))
                    {
                        matchedFiles.Add(new FileReference(file));
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error($"An error occurred: {ex.Message}");
            }
        }

        return matchedFiles;
    }

    public override FileReference[] GetFiles(string pattern, bool recursive = true, bool useRegex = false)
    {
        var searchResult = SearchFiles(pattern, recursive: recursive, useRegex: useRegex);
        return searchResult.ToArray();
    }

    public override bool Exists()
    {
        return Directory.Exists(FullName);
    }


    public DirectoryReference Create()
    {
        Directory.CreateDirectory(FullName);
        return this;
    }
}