using SandboxPipeWorker.GenerateProject.CppProject;

namespace SandboxPipeWorker.GenerateProject;

interface IProjectGenerator
{
    bool GenerateProjectFiles(PlatformProjectGeneratorCollection platformProjectGeneratorCollection);
    bool WritePrimaryProjectFile(ProjectFile projectFile);

    public static string PrimaryProjectName = "Sandbox";
}