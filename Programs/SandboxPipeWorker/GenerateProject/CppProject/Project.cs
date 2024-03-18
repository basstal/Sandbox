using System.Security.Cryptography;
using System.Text;
using SandboxPipeWorker.Common;
using YamlDotNet.RepresentationModel;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class Project
{
    internal static string ProjectFileExtension = ".project.yaml";
    internal static string SearchHeaderFileRegex = ".*\\.h(pp)?";

    public readonly HashSet<ModuleType> CompilableModuleTypes = new()
    {
        ModuleType.Cpp
    };

    public ProjectType ProjectType;
    public CppSubType CppSubType;
    public string PrimaryProjectName;
    public CompileEnvironment PrimaryCompileEnvironment = new();

    public IEnumerable<Module> AllDependencies => EnumerateDependencies();

    public string Guid
    {
        get
        {
            if (GeneratedProjectPath != null)
            {
                return CreateGuidFromPath(GeneratedProjectPath.FullName).ToString();
            }

            return CreateGuidFromPath(ProjectDirectory!.FullName).ToString(); // 没有对应项目文件则使用项目路径
        }
    }

    public static Guid CreateGuidFromPath(string path)
    {
        // 使用 SHA1 哈希算法从路径生成哈希值
        using (var sha1 = SHA1.Create())
        {
            byte[] pathBytes = Encoding.UTF8.GetBytes(path);
            byte[] hashBytes = sha1.ComputeHash(pathBytes);

            // 将哈希值的前16个字节用作 GUID 的基础
            byte[] guidBytes = new byte[16];
            Array.Copy(hashBytes, guidBytes, 16);

            // 创建并返回一个新的 GUID 对象
            return new Guid(guidBytes);
        }
    }

    // public static readonly Dictionary<ProjectType, string> ProjectTypeGuidMapping = new()
    // {
    //     { ProjectType.Cpp, "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942" },
    //     { ProjectType.CSharp, "FAE04EC0-301F-11D3-BF4B-00C04F79EFBC" },
    //     { ProjectType.Folder, "2150E333-8FDC-42A3-9474-1A3956D46DE8" },
    // };

    public static readonly Dictionary<ProjectType, string> ProjectTypeExtensionMapping = new()
    {
        {
            ProjectType.Cpp, ".vcxproj"
        },
        {
            ProjectType.CSharp, ".csproj"
        },
    };

    // public string SlnProjectTypeGuid => ProjectTypeGuidMapping[ProjectType];
    public FileReference? ParsedFile;
    public DirectoryReference? ProjectDirectory;

    public FileReference? GeneratedProjectPath;
    public List<Project> SubProjects = new();

    public static Dictionary<string, Project> RegisteredProjects = new Dictionary<string, Project>();
    public PrecompileEnvironment? PrecompileEnvironment;

    public List<FileReference> RawFiles = new List<FileReference>();

    public Project(string name)
    {
        PrimaryProjectName = name;
        CppSubType = CppSubType.None;
    }

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
            // AddIncludePaths(compileEnvironment.IncludePaths);
            // TODO: 一个 Module 一个 Version 是支持的吗？
            PrimaryCompileEnvironment.CppVersion = compileEnvironment.CppVersion;
            PrimaryCompileEnvironment.Dependencies.Add(module);
        }
    }

    public void AddSourceFile(FileReference sourceFile)
    {
        PrimaryCompileEnvironment.SourceFiles.Add(sourceFile);
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

    public static Project Parser(FileReference fileReference)
    {
        var name = fileReference.GetFileNameWithoutExtension(ProjectFileExtension);
        Project project;
        if (!RegisteredProjects.TryGetValue(name, out project!))
        {
            project = new Project(name);
            RegisteredProjects.Add(name, project);
        }

        if (project.CppSubType != CppSubType.None)
        {
            throw new Exception($"Project {project.PrimaryProjectName} already parsed as {project.CppSubType} from {project.ParsedFile}!");
        }


        var yaml = new YamlStream();
        using (var reader = new StreamReader(fileReference.FullName))
        {
            yaml.Load(reader);
        }

        var root = (YamlMappingNode)yaml.Documents[0].RootNode;
        Enum.TryParse(root["type"].ToString(), out project.ProjectType);
        Enum.TryParse(root["sub_type"].ToString(), out project.CppSubType);
        project.ParsedFile = fileReference;
        project.ProjectDirectory = fileReference.GetDirectory();
        if (ProjectTypeExtensionMapping.ContainsKey(project.ProjectType))
        {
            project.GeneratedProjectPath = fileReference.GetDirectory().GetFile($"{project.PrimaryProjectName}{ProjectTypeExtensionMapping[project.ProjectType]}");
        }

        switch (project.CppSubType)
        {
            case CppSubType.Source:
                project.ParseCppModule(root, fileReference.GetDirectory());
                break;
            case CppSubType.Library:
                project.ParseDynamicLibrary(root, fileReference.GetDirectory());
                project.ParseStaticLibrary(root, fileReference.GetDirectory());
                project.ParseHeaderLibrary(root, fileReference.GetDirectory());
                break;
            default:
                throw new Exception($"Module type {project.CppSubType} not specified!\n{fileReference.FullName}");
        }

        return project;
    }


    internal void ParseHeaderLibrary(YamlMappingNode root, DirectoryReference sourceDirectory)
    {
        PrecompileEnvironment ??= new PrecompileEnvironment();
        if (ProjectDirectory == null)
        {
            throw new Exception("SourceDirectory is null!");
        }

        PrecompileEnvironment.IncludePaths.AddRange(ReadIncludePaths(root));
        var additionalIncludePaths = ReadAdditionalIncludePaths(root);

        foreach (var additionalIncludePath in additionalIncludePaths)
        {
            RawFiles.AddRange(additionalIncludePath.GetFiles(SearchHeaderFileRegex, useRegex: true));
        }

        PrecompileEnvironment.AdditionalIncludePaths.AddRange(additionalIncludePaths);
    }

    internal void ParseStaticLibrary(YamlMappingNode root, DirectoryReference sourceDirectory)
    {
        PrecompileEnvironment ??= new PrecompileEnvironment();

        if (root.Children.ContainsKey("lib_paths") && root["lib_paths"] is YamlSequenceNode libPaths)
        {
            PrecompileEnvironment.LibPaths.AddRange(libPaths.Select(x => sourceDirectory.GetFile(x.ToString())));
        }
    }

    internal void ParseDynamicLibrary(YamlMappingNode root, DirectoryReference sourceDirectory)
    {
        PrecompileEnvironment ??= new PrecompileEnvironment();
        if (root.Children.ContainsKey("dll_paths") && root["dll_paths"] is YamlSequenceNode dllPaths)
        {
            PrecompileEnvironment.DllPaths.AddRange(dllPaths.Select(x => sourceDirectory.GetFile(x.ToString())));
        }
    }

    internal List<FileReference> ReadIncludePaths(YamlMappingNode yamlMappingNode)
    {
        var result = new List<FileReference>();
        if (yamlMappingNode.Children.ContainsKey("include_paths") && yamlMappingNode["include_paths"] is YamlSequenceNode includePathsSequence)
        {
            foreach (var includePath in includePathsSequence)
            {
                var rawReference = FileSystemBase.Create(includePath.ToString(), ProjectDirectory!.FullName);
                result.AddRange(rawReference.GetFiles(SearchHeaderFileRegex, useRegex: true));
            }
        }

        return result;
    }

    internal List<DirectoryReference> ReadAdditionalIncludePaths(YamlMappingNode yamlMappingNode)
    {
        var result = new List<DirectoryReference>();

        if (yamlMappingNode.Children.ContainsKey("additional_include_paths") && yamlMappingNode["additional_include_paths"] is YamlSequenceNode additionalIncludePathsSequence)
        {
            foreach (var additionalIncludePath in additionalIncludePathsSequence)
            {
                result.Add(ProjectDirectory!.Combine(additionalIncludePath.ToString()));
            }
        }

        return result;
    }

    internal List<FileReference> ReadSourcePaths(YamlMappingNode yamlMappingNode)
    {
        var result = new List<FileReference>();
        if (yamlMappingNode.Children.ContainsKey("source_paths") && yamlMappingNode["source_paths"] is YamlSequenceNode sourcePaths)
        {
            foreach (var sourcePath in sourcePaths)
            {
                var rawReference = FileSystemBase.Create(sourcePath.ToString(), ProjectDirectory!.FullName);
                result.AddRange(rawReference.GetFiles("*.cpp"));
            }
        }
        else
        {
            // 扫描 SourceDirectory 下所有 cpp 文件
            var sourceFiles = ProjectDirectory!.SearchFiles("*.cpp");
            result.AddRange(sourceFiles);
        }

        return result;
    }

    internal void ParseCppModule(YamlMappingNode root, DirectoryReference sourceDirectory)
    {
        var compileEnvironment = root["compile_environment"];
        {
            var compileEnvironmentMapping = (YamlMappingNode)compileEnvironment;
            if (compileEnvironmentMapping["definitions"] is YamlSequenceNode definitionsSequence)
            {
                foreach (var definition in definitionsSequence)
                {
                    PrimaryCompileEnvironment.Definitions.Add(definition.ToString());
                }
            }

            if (Enum.TryParse(compileEnvironmentMapping["build_type"].ToString(), out BuildType buildType))
            {
                PrimaryCompileEnvironment.BuildType = buildType;
            }

            PrimaryCompileEnvironment.IncludePaths.AddRange(ReadIncludePaths(compileEnvironmentMapping));
            PrimaryCompileEnvironment.AdditionalIncludePaths.AddRange(ReadAdditionalIncludePaths(compileEnvironmentMapping));

            var cppVersion = compileEnvironmentMapping["cpp_version"];
            Enum.TryParse(cppVersion.ToString(), out PrimaryCompileEnvironment.CppVersion);

            var dependencies = compileEnvironmentMapping["dependencies"];
            if (dependencies is YamlSequenceNode dependenciesSequence)
            {
                foreach (var dependency in dependenciesSequence)
                {
                    var dependencyName = dependency.ToString();
                    if (RegisteredProjects.TryGetValue(dependencyName, out var dependProject))
                    {
                        PrimaryCompileEnvironment.ProjectDependencies.Add(dependProject);
                    }
                    else
                    {
                        dependProject = new Project(dependencyName);
                        RegisteredProjects.Add(dependencyName, dependProject);
                        PrimaryCompileEnvironment.ProjectDependencies.Add(dependProject);
                    }
                }
            }

            PrimaryCompileEnvironment.SourceFiles.AddRange(ReadSourcePaths(compileEnvironmentMapping));
        }
    }

    public void GenerateVcxproj()
    {
        string content = File.ReadAllText("ScribanTemplates/vcxproj.scriban");
        var vcxprojTemplate = Scriban.Template.Parse(content);
        var cppSourceInfos = new List<CppSourceInfo>();
        var sourceRelativeTo = ProjectDirectory!.FullName;

        var includeDirectoryReferences = PrimaryCompileEnvironment.AdditionalIncludePaths.ToList();
        var additionalIncludeDirectoriesParameter = string.Join(";", includeDirectoryReferences.Select(directory => directory.FullName));
        var additionalDependencies = PrimaryCompileEnvironment.ProjectDependencies.SelectMany(module => module.PrecompileEnvironment?.LibPaths ?? FileReference.EmptyList).ToList();
        var dllPaths = PrimaryCompileEnvironment.ProjectDependencies.SelectMany(module => module.PrecompileEnvironment?.DllPaths ?? FileReference.EmptyList).ToList();
        cppSourceInfos.AddRange(
            PrimaryCompileEnvironment.SourceFiles.Select(file => new CppSourceInfo(file.GetRelativePath(sourceRelativeTo), additionalIncludeDirectoriesParameter)));
        foreach (var dependency in PrimaryCompileEnvironment.ProjectDependencies.Where(project => project.CppSubType != CppSubType.None))
        {
            includeDirectoryReferences.AddRange(dependency.PrimaryCompileEnvironment.AdditionalIncludePaths);
            if (dependency.PrecompileEnvironment != null)
            {
                includeDirectoryReferences.AddRange(dependency.PrecompileEnvironment.AdditionalIncludePaths);
            }

            // var dependencyAdditionalIncludeDirectoriesParameter = string.Join(";", dependencyDirectoryReferences.Select(directory => directory.FullName));
            // dependency.PrimaryCompileEnvironment.SourceFiles.ForEach(file => cppSourceInfos.Add(new CppSourceInfo(file.GetRelativePath(sourceRelativeTo), dependencyAdditionalIncludeDirectoriesParameter)));
            additionalDependencies.AddRange(
                dependency.PrimaryCompileEnvironment.ProjectDependencies.SelectMany(module => module.PrecompileEnvironment?.LibPaths ?? FileReference.EmptyList));
            dllPaths.AddRange(dependency.PrimaryCompileEnvironment.ProjectDependencies.SelectMany(module => module.PrecompileEnvironment?.DllPaths ?? FileReference.EmptyList));
        }

        var outputDir = Sandbox.RootDirectory.Combine("Output").FullName;
        var postBuildCommandsTemplate = Scriban.Template.Parse(File.ReadAllText("ScribanTemplates/PostBuildCommands.ps1.scriban"));
        string postBuildCommands = postBuildCommandsTemplate.Render(new
        {
            Project = this,
            OutputDir = outputDir,
            CopyDllPaths = dllPaths.Select(file => file.FullName),
            VulkanSdk = Environment.GetEnvironmentVariable("VULKAN_SDK") // TODO: Vulkan SDK from environment variable
        }, member => member.Name);
        var postBuildCommandsPath = ProjectDirectory.GetFile("PostBuildCommands.ps1").FullName;
        var referenceProjects = PrimaryCompileEnvironment.ProjectDependencies.Where(dependProject => dependProject.ProjectType == ProjectType.Cpp)
            .Select(dependProject => new
            {
                dependProject.PrimaryProjectName,
                dependProject.Guid,
                RelativePathToProject = dependProject.GeneratedProjectPath!.GetRelativePath(ProjectDirectory.FullName)
            });
        RawFiles.Add(ParsedFile!);
        RawFiles.Add(Sandbox.RootDirectory.GetFile(".editorconfig"));
        // 添加着色器源码
        RawFiles.AddRange(ProjectDirectory.GetFiles("*.frag"));
        RawFiles.AddRange(ProjectDirectory.GetFiles("*.vert"));

        File.WriteAllText(postBuildCommandsPath, postBuildCommands);
        var constanst = new
        {
            Platform = "x64",
            BuildTypeMapping = new Dictionary<BuildType, string>()
            {
                {
                    BuildType.Main, "Application"
                },
                {
                    BuildType.Static, "StaticLibrary"
                },
                {
                    BuildType.Dynamic, "DynamicLibrary"
                },
            }
        };
        string primaryProjectFile = vcxprojTemplate.Render(new
        {
            // 启用 多处理器编译
            AllowMultiProcessorCompilation = true,
            Project = this,
            CppSourceInfos = cppSourceInfos,
            RelativeHeaderPaths = PrimaryCompileEnvironment.IncludePaths.Select(include => include.GetRelativePath(sourceRelativeTo)),
            Configurations = new[]
            {
                "Debug", "Release"
            },
            AdditionalIncludeDirectories = string.Join(";", includeDirectoryReferences.Distinct().Select(directory => directory.FullName)),
            AdditionalOptions = string.Join(" ", new[]
            {
                "/wd4275", // 忽略 yamlcpp 的警告
                "/wd4251", // 忽略 yamlcpp 的警告
                "/Zc:__cplusplus", // Boost.hana requires __cplusplus https://github.com/boostorg/hana/issues/516
            }),
            // TODO:Only windows have Dbghelp.lib
            AdditionalDependencies = string.Join(";", additionalDependencies.Distinct().Select(file => file.FullName)) + ";Dbghelp.lib",
            PostBuildCommandsPath = postBuildCommandsPath,
            OutputDir = outputDir,
            ReferenceProjects = referenceProjects,
            RawFiles = RawFiles.Count >= 1000 // 避免超过模板引擎 foreach 上限
                ? RawFiles.GetRange(0, 999)
                : RawFiles,
            Constants = constanst,
        }, member => member.Name);
        File.WriteAllText(GeneratedProjectPath!.FullName, primaryProjectFile);
    }

    public void ScanSubProjects()
    {
        var subProjectFiles = ProjectDirectory!.SearchFiles($"*{ProjectFileExtension}");
        foreach (var subProjectFile in subProjectFiles)
        {
            Project subProject = Parser(subProjectFile);
            AddProject(subProject);
            subProject.AddModules();
        }
    }

    public void GenerateSubProjects()
    {
        foreach (var subProject in SubProjects)
        {
            // if (subProject.ProjectType == ProjectType.Cpp)
            // {
            subProject.GenerateVcxproj();
            // }
        }
    }
}
