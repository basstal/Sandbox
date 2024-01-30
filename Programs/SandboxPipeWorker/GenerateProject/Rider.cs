using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject.CppProject;

namespace SandboxPipeWorker.GenerateProject;

public class Rider : IProjectGenerator
{
    public bool GenerateProjectFiles(PlatformProjectGeneratorCollection platformProjectGeneratorCollection)
    {
        Project rootProject = new Project();

        Project sourceFolder = new Project();
        sourceFolder.ProjectDirectory = Sandbox.RootDirectory.Combine("Sources");
        sourceFolder.PrimaryProjectName = "Sources";

        sourceFolder.ProjectType = ProjectType.Folder;
        rootProject.AddProject(sourceFolder);

        Project programFolder = new Project();
        programFolder.ProjectDirectory = Sandbox.RootDirectory.Combine("Programs");
        programFolder.PrimaryProjectName = "Programs";
        programFolder.ProjectType = ProjectType.Folder;
        rootProject.AddProject(programFolder);


        Project sourceProject = new Project();
        sourceProject.ProjectType = ProjectType.Cpp;
        sourceProject.PrimaryProjectName = "Sandbox";
        sourceProject.ProjectDirectory = sourceFolder.ProjectDirectory;
        sourceProject.GeneratedProjectPath = sourceProject.ProjectDirectory.GetFile(
            $"{sourceProject.PrimaryProjectName}{Project.ProjectTypeExtensionMapping[sourceProject.ProjectType]}");
        sourceProject.AddModules();
        GenerateVcxproj(sourceProject);
        sourceFolder.AddProject(sourceProject);

        Project sandboxPipeWorker = new Project();
        sandboxPipeWorker.ProjectType = ProjectType.CSharp;
        sandboxPipeWorker.PrimaryProjectName = "SandboxPipeWorker";
        sandboxPipeWorker.ProjectDirectory = programFolder.ProjectDirectory.Combine("SandboxPipeWorker");
        sandboxPipeWorker.GeneratedProjectPath = sandboxPipeWorker.ProjectDirectory.GetFile(
            $"{sandboxPipeWorker.PrimaryProjectName}{Project.ProjectTypeExtensionMapping[sandboxPipeWorker.ProjectType]}");
        programFolder.AddProject(sandboxPipeWorker);

        WritePrimaryProjectFile(rootProject);
        return true;
    }

    public bool WritePrimaryProjectFile(Project project)
    {
        string txtTemplate = File.ReadAllText("ScribanTemplates/sln.scriban");
        var template = Scriban.Template.Parse(txtTemplate);
        var projects = project.EnumerateSubProjects().ToList();
        string primaryProjectFile = template.Render(new
        {
            Projects = projects,
            FolderProjects = projects.Where(proj => proj.ProjectType == ProjectType.Folder),
            Configurations = new[]
            {
                "Debug", "Release"
            },
        }, member => member.Name);
        File.WriteAllText(Sandbox.RootDirectory.GetFile("Sandbox.sln").FullName, primaryProjectFile);
        return true;
    }

    class CppSourceInfo
    {
        // ReSharper disable once NotAccessedField.Local
        public string SourcePath;

        // ReSharper disable once NotAccessedField.Local
        public string AdditionalIncludeDirectories;

        public CppSourceInfo(string sourcePath, string additionalIncludeDirectories)
        {
            SourcePath = sourcePath;
            AdditionalIncludeDirectories = additionalIncludeDirectories;
        }
    }

    public void GenerateVcxproj(Project project)
    {
        string txtTemplate = File.ReadAllText("ScribanTemplates/vcxproj.scriban");
        var template = Scriban.Template.Parse(txtTemplate);
        var cppSourceInfos = new List<CppSourceInfo>();
        var sourceRelativeTo = project.ProjectDirectory!.FullName;

        var additionalIncludeDirectories = project.PrimaryCompileEnvironment.EnumerateAdditionalIncludeDirectories();
        var includeDirectoryReferences = additionalIncludeDirectories.ToList();
        var additionalIncludeDirectoriesParameter = string.Join(";", includeDirectoryReferences.Select(directory => directory.FullName));
        var additionalDependencies = project.PrimaryCompileEnvironment.Dependencies.SelectMany(module => module.PrecompileEnvironment?.LibPaths ?? FileReference.EmptyList).ToList();
        var dllPaths = project.PrimaryCompileEnvironment.Dependencies.SelectMany(module => module.PrecompileEnvironment?.DllPaths ?? FileReference.EmptyList).ToList();
        cppSourceInfos.AddRange(project.PrimaryCompileEnvironment.SourceFiles.Select(file =>
            new CppSourceInfo(file.GetRelativePath(sourceRelativeTo), additionalIncludeDirectoriesParameter)));
        foreach (var dependency in project.PrimaryCompileEnvironment.Dependencies.Where(module =>
                     module.Type == ModuleType.Cpp))
        {
            var dependencyAdditionalIncludeDirectories = dependency.CompileEnvironment!.EnumerateAdditionalIncludeDirectories();
            var dependencyDirectoryReferences = dependencyAdditionalIncludeDirectories.ToList();
            includeDirectoryReferences.AddRange(dependencyDirectoryReferences);
            var dependencyAdditionalIncludeDirectoriesParameter = string.Join(";", dependencyDirectoryReferences.Select(directory => directory.FullName));
            dependency.CompileEnvironment!.SourceFiles.ForEach(file =>
                cppSourceInfos.Add(new CppSourceInfo(file.GetRelativePath(sourceRelativeTo), dependencyAdditionalIncludeDirectoriesParameter)));
            additionalDependencies.AddRange(dependency.CompileEnvironment!.Dependencies.SelectMany(module => module.PrecompileEnvironment?.LibPaths ?? FileReference.EmptyList));
            dllPaths.AddRange(dependency.CompileEnvironment!.Dependencies.SelectMany(module => module.PrecompileEnvironment?.DllPaths ?? FileReference.EmptyList));
        }

        var outputDir = Sandbox.RootDirectory.Combine("Output").FullName;
        var postBuildCommandsTemplate = Scriban.Template.Parse(File.ReadAllText("ScribanTemplates/PostBuildCommands.ps1.scriban"));
        string postBuildCommands = postBuildCommandsTemplate.Render(new
        {
            Project = project,
            OutputDir = outputDir,
            CopyDllPaths = dllPaths.Select(file => file.FullName),
            VulkanSdk = Environment.GetEnvironmentVariable("VULKAN_SDK") // TODO: Vulkan SDK from environment variable
        }, member => member.Name);
        var postBuildCommandsPath = project.ProjectDirectory.GetFile("PostBuildCommands.ps1").FullName;
        File.WriteAllText(postBuildCommandsPath, postBuildCommands);
        string primaryProjectFile = template.Render(new
        {
            Project = project,
            CppSourceInfos = cppSourceInfos,
            RelativeHeaderPaths =
                project.PrimaryCompileEnvironment.IncludePaths.SelectMany(include =>
                    include.SearchFiles("*.hpp")
                        .Select(file => file.GetRelativePath(sourceRelativeTo))),
            Configurations = new[]
            {
                "Debug", "Release"
            },
            AdditionalIncludeDirectories = string.Join(";", includeDirectoryReferences.Distinct().Select(directory => directory.FullName)),
            AdditionalDependencies = string.Join(";", additionalDependencies.Distinct().Select(file => file.FullName)),
            PostBuildCommandsPath = postBuildCommandsPath,
            OutputDir = outputDir
        }, member => member.Name);
        File.WriteAllText(project.GeneratedProjectPath!.FullName, primaryProjectFile);
    }
}