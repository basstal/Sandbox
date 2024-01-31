namespace SandboxPipeWorker.GenerateProject.CppProject;

public class CppSourceInfo
{
    // ReSharper disable once NotAccessedField.Local
    public string SourcePath;

    // ReSharper disable once NotAccessedField.Local
    public string AdditionalIncludeDirectories;

    public CppSourceInfo(string sourcePath, string additionalIncludeDirectories)
    {
        SourcePath = sourcePath;
        AdditionalIncludeDirectories = additionalIncludeDirectories;
    }
}