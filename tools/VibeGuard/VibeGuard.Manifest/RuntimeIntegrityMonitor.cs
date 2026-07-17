using System.ComponentModel;
using System.Diagnostics;

namespace VibeGuard.Manifest;

internal sealed class RuntimeIntegrityMonitor
{
    private readonly string root;
    private readonly Dictionary<string, ManifestEntry> entries;
    private readonly Dictionary<string, (long Length, DateTime LastWriteUtc)> snapshots;
    private bool moduleInspectionWarningWritten;

    public RuntimeIntegrityMonitor(string gameDirectory, IntegrityManifest manifest)
    {
        root = SafeGamePath.NormalizeRoot(gameDirectory);
        entries = manifest.Files.ToDictionary(
            entry => SafeGamePath.NormalizeRelative(entry.Path),
            StringComparer.OrdinalIgnoreCase);
        snapshots = new Dictionary<string, (long, DateTime)>(StringComparer.OrdinalIgnoreCase);
        foreach (var relativePath in entries.Keys)
        {
            var info = new FileInfo(SafeGamePath.ResolveFile(root, relativePath));
            snapshots[relativePath] = (info.Length, info.LastWriteTimeUtc);
        }
    }

    public async Task<(bool Valid, string? Reason)> VerifyAsync(Process? gameProcess)
    {
        foreach (var (relativePath, entry) in entries)
        {
            FileInfo info;
            try
            {
                info = new FileInfo(SafeGamePath.ResolveFile(root, relativePath));
                if (!info.Exists)
                    return (false, $"arquivo protegido ausente: {relativePath}");
            }
            catch (FileNotFoundException)
            {
                return (false, $"arquivo protegido ausente: {relativePath}");
            }

            if (info.Length != entry.Length)
                return (false, $"tamanho alterado: {relativePath}");

            var previous = snapshots[relativePath];
            if (previous.Length == info.Length && previous.LastWriteUtc == info.LastWriteTimeUtc)
                continue;

            var hash = await FileHash.Sha256Async(info.FullName);
            if (!string.Equals(hash, entry.Sha256, StringComparison.OrdinalIgnoreCase))
                return (false, $"conteudo alterado: {relativePath}");
            snapshots[relativePath] = (info.Length, info.LastWriteTimeUtc);
        }

        foreach (var candidate in Directory.EnumerateFiles(root, "*", SearchOption.TopDirectoryOnly))
        {
            var relativePath = Path.GetRelativePath(root, candidate);
            if (!entries.ContainsKey(relativePath) && IsPortableExecutable(candidate))
                return (false, $"modulo executavel nao aprovado: {relativePath}");
        }

        if (gameProcess is not null)
        {
            try
            {
                foreach (ProcessModule module in gameProcess.Modules)
                {
                    var modulePath = Path.GetFullPath(module.FileName);
                    var relativePath = Path.GetRelativePath(root, modulePath);
                    if (!relativePath.StartsWith(".." + Path.DirectorySeparatorChar, StringComparison.Ordinal)
                        && !Path.IsPathRooted(relativePath)
                        && !entries.ContainsKey(relativePath))
                        return (false, $"modulo carregado nao aprovado: {relativePath}");
                }
            }
            catch (Exception exception) when (exception is Win32Exception or InvalidOperationException or NotSupportedException)
            {
                if (!moduleInspectionWarningWritten)
                {
                    Console.WriteLine("Inspecao de modulos carregados indisponivel por UAC; monitoramento continuo dos arquivos permanece ativo.");
                    moduleInspectionWarningWritten = true;
                }
            }
        }

        return (true, null);
    }

    private static bool IsPortableExecutable(string path)
    {
        try
        {
            using var stream = new FileStream(path, FileMode.Open, FileAccess.Read,
                FileShare.ReadWrite | FileShare.Delete);
            return stream.ReadByte() == 'M' && stream.ReadByte() == 'Z';
        }
        catch (Exception exception) when (exception is IOException or UnauthorizedAccessException)
        {
            return false;
        }
    }
}
