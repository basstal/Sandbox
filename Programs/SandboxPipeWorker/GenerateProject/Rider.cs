using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject.CppProject;

namespace SandboxPipeWorker.GenerateProject;

public class Rider : IProjectGenerator
{
    public bool GenerateProjectFiles(PlatformProjectGeneratorCollection platformProjectGeneratorCollection)
    {
        Project rootProject = new Project("Sandbox");

        Project sourceFolder = new Project("Sources");
        sourceFolder.ProjectDirectory = Sandbox.RootDirectory.Combine("Sources");
        sourceFolder.ProjectType = ProjectType.Folder;
        rootProject.AddProject(sourceFolder);

        Project programFolder = new Project("Programs");
        programFolder.ProjectDirectory = Sandbox.RootDirectory.Combine("Programs");
        programFolder.ProjectType = ProjectType.Folder;
        rootProject.AddProject(programFolder);

        Project pluginFolder = new Project("Plugins");
        pluginFolder.ProjectDirectory = Sandbox.RootDirectory.Combine("Plugins");
        pluginFolder.ProjectType = ProjectType.Folder;
        rootProject.AddProject(pluginFolder);

        sourceFolder.ScanSubProjects();
        pluginFolder.ScanSubProjects();

        sourceFolder.GenerateSubProjects();
        pluginFolder.GenerateSubProjects();

        Project sandboxPipeWorker = new Project("SandboxPipeWorker");
        sandboxPipeWorker.ProjectType = ProjectType.CSharp;
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
}