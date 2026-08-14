namespace VibeGuard.Manifest;

internal static class GraphicsPreference
{
    private const string FileName = "vibeguard-graphics.ini";

    public static bool LoadUseVSync(string root)
    {
        var path = Path.Combine(root, FileName);
        if (!File.Exists(path))
            return true;

        foreach (var line in File.ReadLines(path))
        {
            var parts = line.Split('=', 2, StringSplitOptions.TrimEntries);
            if (parts.Length == 2
                && parts[0].Equals("UseVSync", StringComparison.OrdinalIgnoreCase))
                return parts[1] != "0";
        }

        return true;
    }

    public static void SaveUseVSync(string root, bool useVSync)
    {
        var path = Path.Combine(root, FileName);
        var temporaryPath = path + ".tmp";
        File.WriteAllText(
            temporaryPath,
            $"[VibeGuard.Graphics]{Environment.NewLine}UseVSync={(useVSync ? 1 : 0)}{Environment.NewLine}");
        File.Move(temporaryPath, path, true);
    }
}
