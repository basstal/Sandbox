using SandboxPipeWorker.Common;

namespace SandboxPipeWorker.GenerateProject.CppProject;

public class SourceFile
{
    public SourceFile(FileReference reference, DirectoryReference baseFolder)
    {
        Reference = reference;
        BaseFolder = baseFolder;
    }

    public SourceFileType FileType;
    public readonly FileReference Reference;
    public readonly DirectoryReference BaseFolder;

    public override string? ToString()
    {
        return Reference.ToString();
    }
}