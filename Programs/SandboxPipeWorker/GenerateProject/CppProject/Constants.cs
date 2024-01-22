﻿namespace SandboxPipeWorker.GenerateProject.CppProject;

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
    MiscLibrary = 4,
    HeaderOnlyLibrary = 5,
}