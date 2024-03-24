using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject.CppProject;

namespace SandboxPipeWorker.GenerateProject;

public class CMake : IProjectGenerator
{
    public bool GenerateProjectFiles(PlatformProjectGeneratorCollection platformProjectGeneratorCollection)
    {
        CMakeProject project = new CMakeProject($"{Sandbox.PrimaryProjectName}Root");
        project.ProjectDirectory = Sandbox.RootDirectory;
        project.ProjectType = ProjectType.Folder;
        project.ScanSubProjects();
        project.GenerateSubProjects();

        WritePrimaryProjectFile(project);
        return true;
    }

    public bool WritePrimaryProjectFile(Project project)
    {
        string cmakeListsTxtTemplate = Sandbox.SourceDirectory.GetFile("ScribanTemplates/CMakeLists.txt.scriban").ReadAllText();
        var template = Scriban.Template.Parse(cmakeListsTxtTemplate);
        var projects = project.EnumerateSubProjects().ToList();
        var subDirectories = projects.Select(p => p.ProjectDirectory!.GetRelativePath(Sandbox.RootDirectory.FullName)).ToList();
        string primaryProjectFile = template.Render(new
        {
            SubDirectories = subDirectories,
            Projects = projects,
            Project = project,
            IsPrimaryProject = true,
        }, member => member.Name);
        File.WriteAllText(Sandbox.RootDirectory.GetFile("CMakeLists.txt").FullName, primaryProjectFile);
        return true;
    }
}
