using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class CMakeProject : Project
{

    public CMakeProject(string name) : base(name)
    {
        ProjectInstanceType = typeof(CMakeProject);
    }


    public void GenerateSubProjects()
    {
        foreach (var subProject in SubProjects)
        {
            if (!(subProject is CMakeProject cMakeProject))
            {
                throw new Exception("SubProject is not CMakeProject");
            }

            // if (subProject.ProjectType == ProjectType.Cpp)
            // {
            cMakeProject.GenerateCMakeLists();
            // }
        }
    }


    public void GenerateCMakeLists()
    {
        string rawTemplate = Sandbox.SourceDirectory.GetFile("ScribanTemplates/CMakeLists.txt.scriban").ReadAllText();
        var parsedTemplate = Scriban.Template.Parse(rawTemplate);
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

        var findPackages = new List<string>
        {
            // "Vulkan",
            // "glfw"
        };
        var referenceProjects = PrimaryCompileEnvironment.ProjectDependencies.Where(dependProject => dependProject.ProjectType == ProjectType.Cpp && !findPackages.Contains(dependProject.Name, StringComparer.OrdinalIgnoreCase))
            .Select(dependProject => new
            {
                dependProject.Name,
                dependProject.ProjectDirectory,
                // dependProject.Guid,
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
                    BuildType.Application, "TODO" // cmake use add_executable as Application build
                },
                {
                    BuildType.Static, "STATIC"
                },
                {
                    BuildType.Dynamic, "SHARED"
                },
            }
        };

        // TODO:Only windows have Dbghelp.lib
        var dependLibs = new List<string>{
            "Dbghelp.lib"
        };
        string primaryProjectFile = parsedTemplate.Render(new
        {
            FindPackages = findPackages,
            DependLibs = dependLibs,
            // 启用 多处理器编译
            AllowMultiProcessorCompilation = true,
            Project = this,
            CppSourceInfos = cppSourceInfos,
            RelativeHeaderPaths = PrimaryCompileEnvironment.IncludePaths.Select(include => include.GetRelativePath(sourceRelativeTo)),
            Configurations = new[]
            {
                "Debug", "Release"
            },
            IncludeDirectories = includeDirectoryReferences.Distinct(),
            AdditionalOptions = string.Join(" ", new[]
            {
                "/wd4275", // 忽略 yamlcpp 的警告
                "/wd4251", // 忽略 yamlcpp 的警告
                "/Zc:__cplusplus", // Boost.hana requires __cplusplus https://github.com/boostorg/hana/issues/516
            }),
            AdditionalDependencies = string.Join(";", additionalDependencies.Distinct().Select(file => file.FullName)),
            PreBuildCommandsPath = preBuildCommandsPath,
            PostBuildCommandsPath = postBuildCommandsPath,
            OutputDir = outputDir,
            ReferenceProjects = referenceProjects,
            RawFiles = RawFiles.Count >= 1000 // 避免超过模板引擎 foreach 上限
                ? RawFiles.GetRange(0, 999)
                : RawFiles,
            Constants = constants,
        }, member => member.Name);
        File.WriteAllText(ProjectDirectory.GetFile("CMakeLists.txt").FullName, primaryProjectFile);
    }
}
