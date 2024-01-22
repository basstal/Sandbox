using System.Text;
// using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject.CppProject;

namespace SandboxPipeWorker.GenerateProject;

public class VisualStudio : IProjectGenerator
{
    public bool GenerateProjectFiles(PlatformProjectGeneratorCollection platformProjectGeneratorCollection)
    {
        bool result = true;
        // List<FileReference> allModules = Module.EnumerateModuleFiles().ToList();


        return result;
    }

    public bool WritePrimaryProjectFile(ProjectFile projectFile)
    {
        // string solutionFileName = $"{IProjectGenerator.PrimaryProjectName}.sln";
        StringBuilder solutionFileContents = new StringBuilder();

        solutionFileContents.AppendLine("Microsoft Visual Studio Solution File, Format Version 12.00");
        solutionFileContents.AppendLine("# Visual Studio Version 17");
        solutionFileContents.AppendLine("VisualStudioVersion = 17.0.31314.256");
        solutionFileContents.AppendLine("MinimumVisualStudioVersion = 10.0.40219.1");
        throw new NotImplementedException();
    }
}