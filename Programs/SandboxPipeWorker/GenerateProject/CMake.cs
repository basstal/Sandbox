using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject.CppProject;

namespace SandboxPipeWorker.GenerateProject;

public class CMake : IProjectGenerator
{
    public bool GenerateProjectFiles(PlatformProjectGeneratorCollection platformProjectGeneratorCollection)
    {
        Project project = new Project("Sandbox");
        project.AddModules();
        // TODO:替换绝对路径
        project.AddSourceFile(new FileReference("Source/Main.cpp"));
        WritePrimaryProjectFile(project);
        return true;
    }

    public bool WritePrimaryProjectFile(Project project)
    {
        string cmakeListsTxtTemplate = File.ReadAllText("ScribanTemplates/CMakeLists.txt.scriban");
        var template = Scriban.Template.Parse(cmakeListsTxtTemplate);
        string primaryProjectFile = template.Render(project, member => member.Name);
        File.WriteAllText(Sandbox.RootDirectory.GetFile("CMakeLists.txt").FullName, primaryProjectFile);
        foreach (var entry in Module.RegisteredModules)
        {
            var moduleProjectFile = template.Render(entry.Value, member => member.Name);
            File.WriteAllText(entry.Value.ParsedFile!.GetDirectory().GetFile("CMakeLists.txt").FullName,
                moduleProjectFile);
        }

        return true;
    }
}