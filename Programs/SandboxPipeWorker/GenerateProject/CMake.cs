﻿using SandboxPipeWorker.Common;
using SandboxPipeWorker.GenerateProject.CppProject;

namespace SandboxPipeWorker.GenerateProject;

public class CMake : IProjectGenerator
{
    public bool GenerateProjectFiles(PlatformProjectGeneratorCollection platformProjectGeneratorCollection)
    {
        ProjectFile projectFile = new ProjectFile();
        projectFile.AddModules();
        // TODO:替换绝对路径
        projectFile.AddSourceFile(new FileReference("Source/Main.cpp"));
        WritePrimaryProjectFile(projectFile);
        return true;
    }

    public bool WritePrimaryProjectFile(ProjectFile projectFile)
    {
        string cmakeListsTxtTemplate = File.ReadAllText("ScribanTemplates/CMakeLists.txt.scriban");
        var template = Scriban.Template.Parse(cmakeListsTxtTemplate);
        string primaryProjectFile = template.Render(projectFile, member => member.Name);
        File.WriteAllText(Sandbox.RootDirectory.GetFile("CMakeLists.txt").FullName, primaryProjectFile);
        foreach (var module in projectFile.PrimaryCompileEnvironment.Dependencies)
        {
            var moduleProjectFile = template.Render(module, member => member.Name);
            File.WriteAllText(module.ParsedFile!.GetDirectory().GetFile("CMakeLists.txt").FullName, moduleProjectFile);
        }

        return true;
    }
}