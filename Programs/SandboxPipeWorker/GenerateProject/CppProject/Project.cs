using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class Project
{
    public readonly HashSet<ModuleType> CompilableModuleTypes = new()
    {
        ModuleType.Cpp
    };

    public ProjectType ProjectType = ProjectType.Folder;
    public string PrimaryProjectName = Sandbox.PrimaryProjectName;
    public CompileEnvironment PrimaryCompileEnvironment = new();
    public IEnumerable<Module> AllDependencies => EnumerateDependencies();
    private string _guid = System.Guid.NewGuid().ToString();
    public string Guid => _guid;

    public static readonly Dictionary<ProjectType, string> ProjectTypeGuidMapping = new()
    {
        { ProjectType.Cpp, "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942" },
        { ProjectType.CSharp, "FAE04EC0-301F-11D3-BF4B-00C04F79EFBC" },
        { ProjectType.Folder, "2150E333-8FDC-42A3-9474-1A3956D46DE8" },
    };

    public static readonly Dictionary<ProjectType, string> ProjectTypeExtensionMapping = new()
    {
        { ProjectType.Cpp, ".vcxproj" },
        { ProjectType.CSharp, ".csproj" },
    };

    public string SlnProjectTypeGuid => ProjectTypeGuidMapping[ProjectType];
    public DirectoryReference? ProjectDirectory;
    public FileReference? GeneratedProjectPath;
    public List<Project> SubProjects = new();


    /// <summary>
    /// 递归 CompileEnvironment.Dependencies 获取所有依赖项
    /// </summary>
    /// <returns></returns>
    public IEnumerable<Module> EnumerateDependencies()
    {
        var dependencies = new HashSet<Module>();
        var queue = new Queue<Module>();
        PrimaryCompileEnvironment.Dependencies.ForEach(module => queue.Enqueue(module));
        while (queue.Count > 0)
        {
            var module = queue.Dequeue();
            dependencies.Add(module);
            if (module.CompileEnvironment?.Dependencies == null)
            {
                continue;
            }

            foreach (var dependency in module.CompileEnvironment?.Dependencies!)
            {
                queue.Enqueue(dependency);
            }
        }

        return dependencies;
    }

    public void AddModules()
    {
        var moduleFiles = Module.EnumerateModuleFiles();
        var compilableModules = new HashSet<Module>();
        foreach (FileReference moduleFile in moduleFiles)
        {
            Module module = Module.Parser(moduleFile);
            if (CompilableModuleTypes.Contains(module.Type))
            {
                compilableModules.Add(module);
            }
        }

        ValidateRegisteredModules();

        foreach (var compilableModule in compilableModules)
        {
            AddModule(compilableModule);
        }
    }

    public void ValidateRegisteredModules()
    {
        foreach (var entry in Module.RegisteredModules)
        {
            if (entry.Value.Type == ModuleType.None)
            {
                throw new Exception($"Module {entry.Key} has an invalid type!");
            }
        }
    }

    public void AddModule(Module module)
    {
        var compileEnvironment = module.CompileEnvironment;
        if (compileEnvironment != null)
        {
            AddPreprocessorDefinitions(compileEnvironment.Definitions);
            AddIncludePaths(compileEnvironment.IncludePaths);
            // TODO: 一个 Module 一个 Version 是支持的吗？
            PrimaryCompileEnvironment.CppVersion = compileEnvironment.CppVersion;
            PrimaryCompileEnvironment.Dependencies.Add(module);
        }
    }

    public void AddSourceFile(FileReference sourceFile)
    {
        PrimaryCompileEnvironment.SourceFiles.Add(sourceFile);
    }

    public void AddIncludePaths(List<DirectoryReference> includePaths)
    {
        foreach (DirectoryReference includePath in includePaths)
        {
            PrimaryCompileEnvironment.IncludePaths.Add(includePath);
        }
    }


    public Dictionary<string, string> PreprocessorDefinitions = new();

    public void AddPreprocessorDefinitions(List<string> definitions)
    {
        foreach (string definition in definitions)
        {
            SplitDefinitions(definition, out string name, out string value);

            // Ignore any API macros being import/export; we'll assume they're valid across the whole project
            if (name.EndsWith("_API", StringComparison.Ordinal))
            {
                value = string.Empty;
            }

            PreprocessorDefinitions.TryAdd(name, value);
        }
    }


    void SplitDefinitions(string definition, out string name, out string value)
    {
        int index = definition.IndexOf('=');
        if (index == -1)
        {
            name = definition;
            value = string.Empty;
        }
        else
        {
            name = definition.Substring(0, index);
            value = definition.Substring(index + 1);
        }
    }

    public void AddProject(Project project)
    {
        SubProjects.Add(project);
    }

    public IEnumerable<Project> EnumerateSubProjects()
    {
        foreach (var subProject in SubProjects)
        {
            yield return subProject;
            foreach (var subSubProject in subProject.EnumerateSubProjects())
            {
                yield return subSubProject;
            }
        }
    }

    
}