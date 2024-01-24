using SandboxPipeWorker.GenerateProject.CppProject;

namespace SandboxPipeWorker.GenerateProject;

public class VisualStudio : IProjectGenerator
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
            Configurations = new[] { "Debug", "Release" },
            PlatformsAndArchitectures = new[] { "Any CPU" },
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

        var additionalIncludeDirectories = project.PrimaryCompileEnvironment.GetAdditionalIncludeDirectories();
        cppSourceInfos.AddRange(project.PrimaryCompileEnvironment.SourceFiles.Select(file =>
            new CppSourceInfo(file.GetRelativePath(sourceRelativeTo), additionalIncludeDirectories)));
        foreach (var dependency in project.PrimaryCompileEnvironment.Dependencies.Where(module =>
                     module.Type == ModuleType.Cpp))
        {
            dependency.CompileEnvironment!.SourceFiles.ForEach(file =>
                cppSourceInfos.Add(new CppSourceInfo(file.GetRelativePath(sourceRelativeTo),
                    dependency.CompileEnvironment.GetAdditionalIncludeDirectories())));
        }

        string primaryProjectFile = template.Render(new
        {
            Project = project,
            CppSourceInfos = cppSourceInfos,
            RelativeHeaderPaths =
                project.PrimaryCompileEnvironment.IncludePaths.SelectMany(include =>
                    include.SearchFiles("*.hpp")
                        .Select(file => file.GetRelativePath(sourceRelativeTo))),
            Configurations = new[] { "Debug", "Release" },
        }, member => member.Name);
        File.WriteAllText(project.GeneratedProjectPath!.FullName, primaryProjectFile);
    }
}