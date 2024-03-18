namespace SandboxPipeWorker.GenerateProject.CppProject;

public enum CppVersion
{
    Cpp20,
    Default = Cpp20
}

public enum SourceFileType
{
    Cpp,
    Hpp,
}

public enum ModuleType
{
    None = 0,
    Cpp = 1,
    DynamicLibrary = 2,
    StaticLibrary = 3,
    HeaderOnlyLibrary = 5,
}

public enum ProjectType
{
    None,
    Cpp,
    CSharp,
    Folder,
}

public enum CppSubType
{
    None,
    Source,
    Library,
}
