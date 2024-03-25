using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class VcxProject : Project
{

    public FileReference? GeneratedProjectPath;

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
    public static readonly Dictionary<ProjectType, string> ProjectTypeExtensionMapping = new()
    {
        {
            ProjectType.Cpp, ".vcxproj"
        },
        {
            ProjectType.CSharp, ".csproj"
        },
    };

    public VcxProject(string name) : base(name)
    {
        ProjectInstanceType = typeof(VcxProject);
    }

    public void GenerateVcxproj()
    {
        string content = Sandbox.SourceDirectory.GetFile("ScribanTemplates/vcxproj.scriban").ReadAllText();
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

        var outputDir = Sandbox.RootDirectory.GetDirectory("Output").FullName;

        // 导出构建后执行的命令
        var postBuildCommandsPath = GenerateCommands(outputDir, dllPaths, "PostBuildCommands");
        // 导出构建前执行的命令
        var preBuildCommandsPath = GenerateCommands(outputDir, dllPaths, "PreBuildCommands");

        var referenceProjects = PrimaryCompileEnvironment.ProjectDependencies.Where(dependProject => dependProject.ProjectType == ProjectType.Cpp)
            .Select(dependProject =>
            {
                if (dependProject is VcxProject vcxProject)
                {
                    return new
                    {
                        vcxProject.PrimaryProjectName,
                        vcxProject.Guid,
                        RelativePathToProject = vcxProject.GeneratedProjectPath!.GetRelativePath(ProjectDirectory.FullName)
                    };
                }
                throw new Exception("Project is not VcxProject");
            });
        RawFiles.Add(ParsedFile!);
        RawFiles.Add(Sandbox.RootDirectory.GetFile(".editorconfig"));
        // 添加着色器源码
        RawFiles.AddRange(ProjectDirectory.GetFiles("*.frag"));
        RawFiles.AddRange(ProjectDirectory.GetFiles("*.vert"));

        var constants = new
        {
            Platform = "x64",
            BuildTypeMapping = new Dictionary<BuildType, string>()
            {
                {
                    BuildType.Application, "Application"
                },
                {
                    BuildType.Static, "StaticLibrary"
                },
                {
                    BuildType.Dynamic, "DynamicLibrary"
                },
            }
        };

        // TODO:Only windows have Dbghelp.lib
        var dependLibs = new List<string>{
            "Dbghelp.lib"
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
            AdditionalDependencies = string.Join(";", additionalDependencies.Distinct().Select(file => file.FullName).Concat(dependLibs)),
            PreBuildCommandsPath = preBuildCommandsPath,
            PostBuildCommandsPath = postBuildCommandsPath,
            OutputDir = outputDir,
            ReferenceProjects = referenceProjects,
            RawFiles = RawFiles.Count >= 1000 // 避免超过模板引擎 foreach 上限
                ? RawFiles.GetRange(0, 999)
                : RawFiles,
            Constants = constants,
        }, member => member.Name);
        File.WriteAllText(GeneratedProjectPath!.FullName, primaryProjectFile);
    }

    public void GenerateSubProjects()
    {
        foreach (var subProject in SubProjects)
        {
            if (!(subProject is VcxProject vcxProject))
            {
                throw new Exception("SubProject is not VcxProject");
            }

            // if (subProject.ProjectType == ProjectType.Cpp)
            // {
            vcxProject.GenerateVcxproj();
            // }
        }
    }

    public override void PostProcessParse(FileReference fileReference)
    {
        var fileName = PrimaryProjectName;
        if (ProjectTypeExtensionMapping.ContainsKey(ProjectType))
        {
            fileName += ProjectTypeExtensionMapping[ProjectType];
        }
        GeneratedProjectPath = fileReference.GetDirectory().GetFile(fileName);
    }

}
