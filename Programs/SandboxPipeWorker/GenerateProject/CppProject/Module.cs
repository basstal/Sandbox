using SandboxPipeWorker.Common;
using YamlDotNet.RepresentationModel;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class Module
{
    internal static string ModuleFileExtension = ".module.yaml";
    public string Name;
    public ModuleType Type;
    public FileReference? ParsedFile;
    public DirectoryReference? SourceDirectory => ParsedFile?.GetDirectory();
    public CompileEnvironment? CompileEnvironment;

    public IEnumerable<string>? RelativeModuleSourcePath =>
        SourceDirectory != null
            ? CompileEnvironment?.SourceFiles.Select(file => file.GetRelativePath(SourceDirectory.FullName))
            : null;

    public PrecompileEnvironment? PrecompileEnvironment;

    public static Dictionary<string, Module> RegisteredModules = new Dictionary<string, Module>();

    public Module(string name)
    {
        Name = name;
        Type = ModuleType.None;
    }

    public static Module Parser(FileReference fileReference)
    {
        var name = fileReference.GetFileNameWithoutExtension(ModuleFileExtension);
        Module module;
        if (!RegisteredModules.TryGetValue(name, out module!))
        {
            module = new Module(name);
            RegisteredModules.Add(name, module);
        }

        if (module.Type != ModuleType.None)
        {
            throw new Exception($"Module {module.Name} already parsed as {module.Type} from {module.ParsedFile}!");
        }


        var yaml = new YamlStream();
        using (var reader = new StreamReader(fileReference.FullName))
        {
            yaml.Load(reader);
        }

        var root = (YamlMappingNode)yaml.Documents[0].RootNode;
        Enum.TryParse(root["type"].ToString(), out module.Type);
        module.ParsedFile = fileReference;

        switch (module.Type)
        {
            case ModuleType.Cpp:
                module.ParseCppModule(root, fileReference.GetDirectory());
                break;
            case ModuleType.DynamicLibrary:
                module.ParseDynamicLibrary(root, fileReference.GetDirectory());
                module.ParseStaticLibrary(root, fileReference.GetDirectory());
                module.ParseHeaderOnlyLibrary(root, fileReference.GetDirectory());
                break;
            case ModuleType.StaticLibrary:
                module.ParseStaticLibrary(root, fileReference.GetDirectory());
                module.ParseHeaderOnlyLibrary(root, fileReference.GetDirectory());
                break;
            case ModuleType.HeaderOnlyLibrary:
                module.ParseHeaderOnlyLibrary(root, fileReference.GetDirectory());
                break;
            case ModuleType.None:
                throw new Exception("Module type not specified!");
        }

        return module;
    }

    internal List<FileReference> ReadIncludePaths(YamlMappingNode yamlMappingNode)
    {
        var result = new List<FileReference>();
        if (yamlMappingNode.Children.ContainsKey("include_paths") && yamlMappingNode["include_paths"] is YamlSequenceNode includePathsSequence)
        {
            foreach (var includePath in includePathsSequence)
            {
                var rawReference = FileSystemBase.Create(includePath.ToString(), SourceDirectory!.FullName);
                result.AddRange(rawReference.GetFiles(".*\\.h(pp)?", useRegex: true));
            }
        }

        return result;
    }

    internal void ParseHeaderOnlyLibrary(YamlMappingNode root, DirectoryReference sourceDirectory)
    {
        PrecompileEnvironment ??= new PrecompileEnvironment();
        if (SourceDirectory == null)
        {
            throw new Exception("SourceDirectory is null!");
        }

        PrecompileEnvironment.IncludePaths.AddRange(ReadIncludePaths(root));
    }

    internal void ParseStaticLibrary(YamlMappingNode root, DirectoryReference sourceDirectory)
    {
        PrecompileEnvironment ??= new PrecompileEnvironment();

        if (root["lib_paths"] is YamlSequenceNode libPaths)
        {
            PrecompileEnvironment.LibPaths.AddRange(libPaths.Select(x => sourceDirectory.GetFile(x.ToString())));
        }
    }

    internal void ParseDynamicLibrary(YamlMappingNode root, DirectoryReference sourceDirectory)
    {
        PrecompileEnvironment ??= new PrecompileEnvironment();
        if (root["dll_paths"] is YamlSequenceNode dllPaths)
        {
            PrecompileEnvironment.DllPaths.AddRange(dllPaths.Select(x => sourceDirectory.GetFile(x.ToString())));
        }
    }

    internal void ParseCppModule(YamlMappingNode root, DirectoryReference sourceDirectory)
    {
        var compileEnvironment = root["compile_environment"];
        {
            CompileEnvironment = new CompileEnvironment();
            var compileEnvironmentMapping = (YamlMappingNode)compileEnvironment;
            var definitions = compileEnvironmentMapping["definitions"];
            if (definitions is YamlSequenceNode definitionsSequence)
            {
                foreach (var definition in definitionsSequence)
                {
                    CompileEnvironment.Definitions.Add(definition.ToString());
                }
            }

            var includePaths = compileEnvironmentMapping["include_paths"];
            CompileEnvironment.AdditionalIncludePaths.Add(sourceDirectory);
            if (includePaths is YamlSequenceNode includePathsSequence)
            {
                foreach (var includePath in includePathsSequence)
                {
                    CompileEnvironment.AdditionalIncludePaths.Add(new DirectoryReference(includePath.ToString()));
                }
            }


            var cppVersion = compileEnvironmentMapping["cpp_version"];
            Enum.TryParse(cppVersion.ToString(), out CompileEnvironment.CppVersion);

            var dependencies = compileEnvironmentMapping["dependencies"];
            if (dependencies is YamlSequenceNode dependenciesSequence)
            {
                foreach (var dependency in dependenciesSequence)
                {
                    var dependencyName = dependency.ToString();
                    if (RegisteredModules.TryGetValue(dependencyName, out var dependencyModule))
                    {
                        CompileEnvironment.Dependencies.Add(dependencyModule);
                    }
                    else
                    {
                        dependencyModule = new Module(dependencyName);
                        RegisteredModules.Add(dependencyName, dependencyModule);
                        CompileEnvironment.Dependencies.Add(dependencyModule);
                    }
                }
            }

            // 扫描 SourceDirectory 下所有 cpp 文件
            var sourceFiles = SourceDirectory!.SearchFiles("*.cpp");
            foreach (var file in sourceFiles)
            {
                CompileEnvironment.SourceFiles.Add(file);
            }
        }
    }

    public static IEnumerable<FileReference> EnumerateModuleFiles()
    {
        HashSet<FileReference> moduleFiles = new();

        foreach (DirectoryReference baseDirectory in EnumerateBaseDirectories())
        {
            var searchFiles = baseDirectory.SearchFiles($"*{ModuleFileExtension}");
            foreach (var searchFile in searchFiles)
            {
                // TODO:是否需要检测同名模块
                moduleFiles.Add(searchFile);
            }
        }

        return moduleFiles;
    }

    public static IEnumerable<DirectoryReference> EnumerateBaseDirectories()
    {
        yield return Sandbox.RootDirectory.Combine("Sources");
        yield return Sandbox.RootDirectory.Combine("Plugins");
    }
}