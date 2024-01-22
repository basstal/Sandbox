using Microsoft.Extensions.Logging;

namespace SandboxPipeWorker.Common;

public static class Log
{
    public static ILogger Logger = new DefaultLogger();

    public static void Info(string message)
    {
        Logger.LogInformation(message);
    }

    public static void Warning(string message)
    {
        Logger.LogWarning(message);
    }

    public static void Error(string message)
    {
        Logger.LogError(message);
    }

    public static void Error(Exception exception)
    {
        Logger.LogError(exception, exception.Message);
    }

    public static void Error(string message, Exception exception)
    {
        Logger.LogError(exception, message);
    }
}

/// <summary>
/// Default logger that does nothing.
/// </summary>
class DefaultLogger : ILogger
{
    public void Log<TState>(LogLevel logLevel, EventId eventId, TState state, Exception? exception,
        Func<TState, Exception?, string> formatter)
    {
        formatter(state, exception);
    }

    public bool IsEnabled(LogLevel logLevel)
    {
        return true;
    }

    public IDisposable? BeginScope<TState>(TState state) where TState : notnull
    {
        return null;
    }
}