namespace VibeGuard.Manifest;

internal static class SafeGamePath
{
    public static string NormalizeRoot(string gameDirectory)
    {
        var root = Path.GetFullPath(gameDirectory);
        if (!Directory.Exists(root))
            throw new InvalidOperationException($"Pasta do jogo inexistente: {root}");
        return root;
    }

    public static string NormalizeRelative(string path)
    {
        if (string.IsNullOrWhiteSpace(path) || Path.IsPathRooted(path))
            throw new InvalidOperationException($"Caminho relativo invalido: {path}");
        return path.Replace('/', Path.DirectorySeparatorChar);
    }

    public static string ResolveFile(string root, string relativePath)
    {
        var normalized = NormalizeRelative(relativePath);
        var candidate = Path.GetFullPath(Path.Combine(root, normalized));
        var rootPrefix = root.TrimEnd(Path.DirectorySeparatorChar) + Path.DirectorySeparatorChar;

        if (!candidate.StartsWith(rootPrefix, StringComparison.OrdinalIgnoreCase))
            throw new InvalidOperationException($"O caminho sai da pasta do jogo: {relativePath}");

        var info = new FileInfo(candidate);
        if (!info.Exists)
            throw new FileNotFoundException($"Arquivo obrigatorio ausente: {relativePath}");
        if ((info.Attributes & FileAttributes.ReparsePoint) != 0)
            throw new InvalidOperationException($"Links/reparse points nao sao permitidos: {relativePath}");

        return candidate;
    }
}
