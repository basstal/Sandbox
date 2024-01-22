using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class ProjectFile
{
    public readonly HashSet<ModuleType> CompilableModuleTypes = new()
    {
        ModuleType.Cpp
    };

    public string PrimaryProjectName = Sandbox.PrimaryProjectName;
    public CompileEnvironment PrimaryCompileEnvironment = new();

    public void AddModules()
    {
        var moduleFiles = Module.EnumerateModuleFiles();
        var compilableModules = new HashSet<Module>();
        foreach (FileReference moduleFile in moduleFiles)
        {
            Module module = Module.Parser(moduleFile);
            if (CompilableModuleTypes.Contains(module.Type))
            {
                compilableModules.Add(module);
            }
        }

        foreach (var compilableModule in compilableModules)
        {
            AddModule(compilableModule);
        }
    }

    public void AddModule(Module module)
    {
        var compileEnvironment = module.CompileEnvironment;
        if (compileEnvironment != null)
        {
            PrimaryCompileEnvironment = new CompileEnvironment();
            AddPreprocessorDefinitions(compileEnvironment.Definitions);
            AddIncludePaths(compileEnvironment.IncludePaths);
            // ?? 一个 Module 一个 Version 是支持的吗？
            PrimaryCompileEnvironment.CppVersion = compileEnvironment.CppVersion;
            PrimaryCompileEnvironment.Dependencies.Add(module);
        }
    }

    public void AddSourceFile(FileReference sourceFile)
    {
        PrimaryCompileEnvironment.SourceFiles.Add(sourceFile);
    }

    public void AddIncludePaths(List<DirectoryReference> includePaths)
    {
        foreach (DirectoryReference includePath in includePaths)
        {
            PrimaryCompileEnvironment.IncludePaths.Add(includePath);
        }
    }


    public Dictionary<string, string> PreprocessorDefinitions = new();

    public void AddPreprocessorDefinitions(List<string> definitions)
    {
        foreach (string definition in definitions)
        {
            SplitDefinitions(definition, out string name, out string value);

            // Ignore any API macros being import/export; we'll assume they're valid across the whole project
            if (name.EndsWith("_API", StringComparison.Ordinal))
            {
                value = string.Empty;
            }

            PreprocessorDefinitions.TryAdd(name, value);
        }
    }


    void SplitDefinitions(string definition, out string name, out string value)
    {
        int index = definition.IndexOf('=');
        if (index == -1)
        {
            name = definition;
            value = string.Empty;
        }
        else
        {
            name = definition.Substring(0, index);
            value = definition.Substring(index + 1);
        }
    }
}