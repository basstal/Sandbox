using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class CompileEnvironment
{
    public CppVersion CppVersion = CppVersion.Default;
    public List<string> Definitions = new List<string>();
    public List<FileSystemBase> IncludePaths = new List<FileSystemBase>();
    public List<FileReference> SourceFiles = new List<FileReference>();
    public List<Module> Dependencies = new List<Module>();
    public List<Project> ProjectDependencies = new List<Project>();

    public List<FileSystemBase> DependencyIncludePaths =>
        Dependencies
            .SelectMany(dependency => dependency.CompileEnvironment?.IncludePaths ??
                                      dependency.PrecompileEnvironment?.IncludePaths ?? new List<FileSystemBase>())
            .ToList();

    public IEnumerable<DirectoryReference> EnumerateAdditionalIncludeDirectories()
    {
        var directIncludePaths = IncludePaths.ToList();
        directIncludePaths.AddRange(DependencyIncludePaths);
        return directIncludePaths.OfType<DirectoryReference>().Distinct();
    }
}