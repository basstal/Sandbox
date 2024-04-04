using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject.CppProject;
using SandboxPipeWorker.GenerateProject.Variants;

namespace SandboxPipeWorker.GenerateProject;

public class Rider : IProjectGenerator
{
    public bool GenerateProjectFiles(PlatformProjectGeneratorCollection platformProjectGeneratorCollection)
    {
        Project rootProject = new RiderProject(Sandbox.PrimaryProjectName);

        RiderProject sourceFolder = new RiderProject("Sources");
        sourceFolder.ProjectDirectory = Sandbox.RootDirectory.GetDirectory("Sources");
        sourceFolder.ProjectType = ProjectType.Folder;
        rootProject.AddProject(sourceFolder);

        Project programFolder = new RiderProject("Programs");
        programFolder.ProjectDirectory = Sandbox.RootDirectory.GetDirectory("Programs");
        programFolder.ProjectType = ProjectType.Folder;
        rootProject.AddProject(programFolder);

        RiderProject pluginFolder = new RiderProject("Plugins");
        pluginFolder.ProjectDirectory = Sandbox.RootDirectory.GetDirectory("Plugins");
        pluginFolder.ProjectType = ProjectType.Folder;
        rootProject.AddProject(pluginFolder);

        sourceFolder.ScanSubProjects();
        pluginFolder.ScanSubProjects();

        sourceFolder.GenerateSubProjects();
        pluginFolder.GenerateSubProjects();

        RiderProject sandboxPipeWorker = new RiderProject("SandboxPipeWorker");
        sandboxPipeWorker.ProjectType = ProjectType.CSharp;
        sandboxPipeWorker.ProjectDirectory = programFolder.ProjectDirectory.GetDirectory("SandboxPipeWorker");
        sandboxPipeWorker.GeneratedProjectPath = sandboxPipeWorker.ProjectDirectory.GetFile(
            $"{sandboxPipeWorker.PrimaryProjectName}{RiderProject.ProjectTypeExtensionMapping[sandboxPipeWorker.ProjectType]}");
        programFolder.AddProject(sandboxPipeWorker);

        WritePrimaryProjectFile(rootProject);
        return true;
    }

    public bool WritePrimaryProjectFile(Project project)
    {
        string txtTemplate = Sandbox.SourceDirectory.GetFile("ScribanTemplates/sln.scriban").ReadAllText();
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
        File.WriteAllText(Sandbox.RootDirectory.GetFile($"{project.Name}.sln").FullName, primaryProjectFile);
        return true;
    }
}