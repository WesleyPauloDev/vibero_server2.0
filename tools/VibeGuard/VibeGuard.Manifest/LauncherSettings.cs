using System.Text.Json;

namespace VibeGuard.Manifest;

internal sealed record LauncherSettings(
    string Executable,
    string Server,
    bool AllowInsecureHttp = false,
    int StartupDelayMilliseconds = 5000)
{
    public static LauncherSettings Load(string root)
    {
        var path = Path.Combine(root, "vibeguard-launcher.json");
        var settings = JsonSerializer.Deserialize<LauncherSettings>(
            File.ReadAllText(path), JsonSupport.Options)
            ?? throw new InvalidOperationException("A configuracao do launcher esta vazia ou invalida.");

        if (string.IsNullOrWhiteSpace(settings.Executable))
            throw new InvalidOperationException("O executavel do launcher nao foi configurado.");
        if (string.IsNullOrWhiteSpace(settings.Server))
            throw new InvalidOperationException("O servidor do VibeGuard nao foi configurado.");
        if (settings.StartupDelayMilliseconds is < 0 or > 15000)
            throw new InvalidOperationException("O atraso do launcher deve estar entre 0 e 15000 ms.");

        return settings;
    }
}
