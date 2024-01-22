﻿using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class CompileEnvironment
{
    public CppVersion CppVersion = CppVersion.Default;
    public List<string> Definitions = new List<string>();
    public List<DirectoryReference> IncludePaths = new List<DirectoryReference>();
    public List<FileReference> SourceFiles = new List<FileReference>();
    public List<Module> Dependencies = new List<Module>();
}