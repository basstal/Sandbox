namespace SandboxPipeWorker.GenerateProject.CppProject;

public class CppSourceInfo
{
    // ReSharper disable once NotAccessedField.Local
    public string SourcePath;
    public string ObjectPath => Path.ChangeExtension(SourcePath, ".obj")!;

    // ReSharper disable once NotAccessedField.Local
    public string AdditionalIncludeDirectories;

    public CppSourceInfo(string sourcePath, string additionalIncludeDirectories)
    {
        SourcePath = sourcePath;
        AdditionalIncludeDirectories = additionalIncludeDirectories;
    }
}
