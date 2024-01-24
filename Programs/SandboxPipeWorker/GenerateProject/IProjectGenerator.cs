using SandboxPipeWorker.GenerateProject.CppProject;

namespace SandboxPipeWorker.GenerateProject;

interface IProjectGenerator
{
    bool GenerateProjectFiles(PlatformProjectGeneratorCollection platformProjectGeneratorCollection);
    bool WritePrimaryProjectFile(Project project);

    public static string PrimaryProjectName = "Sandbox";
}