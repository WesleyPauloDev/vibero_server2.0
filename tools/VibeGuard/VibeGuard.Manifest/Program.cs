namespace VibeGuard.Manifest;

internal static class Program
{
    [STAThread]
    private static int Main(string[] args)
    {
        if (args.Length > 0)
            return CommandRunner.RunAsync(args).GetAwaiter().GetResult();

        ApplicationConfiguration.Initialize();
        using var window = new GuardWindow();
        Application.Run(window);
        return window.ExitCode;
    }
}
