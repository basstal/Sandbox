namespace SandboxPipeWorker.Pipes;

interface IPipe
{
    public Task<int> ExecuteAsync();
}