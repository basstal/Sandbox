using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class CompileEnvironment
{
    public CppVersion CppVersion = CppVersion.Default;
    public List<string> Definitions = new List<string>();
    public List<FileReference> IncludePaths = new List<FileReference>();
    public List<DirectoryReference> AdditionalIncludePaths = new List<DirectoryReference>();
    public List<FileReference> SourceFiles = new List<FileReference>();
    public List<Module> Dependencies = new List<Module>();
    public List<Project> ProjectDependencies = new List<Project>();
    public BuildType BuildType = BuildType.None;

    public List<FileReference> DependencyIncludePaths =>
        Dependencies
            .SelectMany(dependency => dependency.CompileEnvironment?.IncludePaths ??
                                      dependency.PrecompileEnvironment?.IncludePaths ?? new List<FileReference>())
            .ToList();

    // public IEnumerable<DirectoryReference> EnumerateAdditionalIncludeDirectories()
    // {
    //     var directIncludePaths = IncludePaths.ToList();
    //     directIncludePaths.AddRange(DependencyIncludePaths);
    //     return directIncludePaths.OfType<DirectoryReference>().Distinct();
    // }
}