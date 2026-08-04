using System.Security.Cryptography;
using System.Text;
using System.Text.Json;

namespace VibeGuard.Manifest;

internal static class ManifestService
{
    public static async Task GenerateAsync(
        string gameDirectory,
        string policyPath,
        string outputPath)
    {
        var root = SafeGamePath.NormalizeRoot(gameDirectory);
        var policy = JsonSerializer.Deserialize<GuardPolicy>(
            await File.ReadAllTextAsync(Path.GetFullPath(policyPath)), JsonSupport.Options)
            ?? throw new InvalidOperationException("A politica esta vazia ou invalida.");

        if (policy.Files.Count == 0)
            throw new InvalidOperationException("A politica precisa listar ao menos um arquivo.");

        var entries = new List<ManifestEntry>();
        var seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

        foreach (var configuredPath in policy.Files)
        {
            var relativePath = SafeGamePath.NormalizeRelative(configuredPath);
            if (!seen.Add(relativePath))
                throw new InvalidOperationException($"Arquivo repetido na politica: {relativePath}");

            var fullPath = SafeGamePath.ResolveFile(root, relativePath);
            var info = new FileInfo(fullPath);
            entries.Add(new ManifestEntry(
                relativePath.Replace('\\', '/'),
                info.Length,
                await FileHash.Sha256Async(fullPath)));
        }

        entries.Sort((left, right) =>
            StringComparer.OrdinalIgnoreCase.Compare(left.Path, right.Path));

        var manifest = new IntegrityManifest(
            1,
            policy.Product,
            "SHA-256",
            entries,
            policy.RejectUnexpectedPortableExecutables);
        var fullOutputPath = Path.GetFullPath(outputPath);
        Directory.CreateDirectory(Path.GetDirectoryName(fullOutputPath)
            ?? throw new InvalidOperationException("Caminho de saida invalido."));
        await File.WriteAllTextAsync(
            fullOutputPath,
            JsonSerializer.Serialize(manifest, JsonSupport.Options) + Environment.NewLine);

        Console.WriteLine($"Manifesto criado com {entries.Count} arquivos.");
        Console.WriteLine($"Saida: {fullOutputPath}");
        Console.WriteLine("Nenhum dado foi enviado pela rede.");
    }

    public static async Task<VerificationResult> VerifyAsync(
        string gameDirectory,
        string manifestPath,
        string signaturePath)
    {
        var root = SafeGamePath.NormalizeRoot(gameDirectory);
        await ManifestSignature.VerifyTrustedAsync(manifestPath, signaturePath);
        var manifest = JsonSerializer.Deserialize<IntegrityManifest>(
            await File.ReadAllTextAsync(Path.GetFullPath(manifestPath)), JsonSupport.Options)
            ?? throw new InvalidOperationException("O manifesto esta vazio ou invalido.");

        ValidateManifest(manifest);
        var valid = true;
        var seen = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

        foreach (var entry in manifest.Files)
        {
            var relativePath = SafeGamePath.NormalizeRelative(entry.Path);
            if (!seen.Add(relativePath))
                throw new InvalidOperationException($"Arquivo repetido no manifesto: {relativePath}");

            try
            {
                var fullPath = SafeGamePath.ResolveFile(root, relativePath);
                var info = new FileInfo(fullPath);
                var actualHash = info.Length == entry.Length
                    ? await FileHash.Sha256Async(fullPath)
                    : string.Empty;
                var hashMatches = FixedTimeHexEquals(actualHash, entry.Sha256);

                if (info.Length == entry.Length && hashMatches)
                    Console.WriteLine($"[OK] {entry.Path}");
                else
                {
                    valid = false;
                    Console.WriteLine($"[ALTERADO] {entry.Path}");
                }
            }
            catch (FileNotFoundException)
            {
                valid = false;
                Console.WriteLine($"[AUSENTE] {entry.Path}");
            }
        }

        if (manifest.RejectUnexpectedPortableExecutables)
        {
            foreach (var candidatePath in Directory.EnumerateFiles(root, "*", SearchOption.TopDirectoryOnly))
            {
                var relativePath = Path.GetRelativePath(root, candidatePath);
                if (seen.Contains(relativePath)
                    || await TrustedUpdaterComponent.RemoveOrRecognizeAsync(candidatePath)
                    || !IsPortableExecutable(candidatePath))
                    continue;

                valid = false;
                Console.WriteLine($"[NAO APROVADO] {relativePath.Replace('\\', '/')}");
            }
        }

        Console.WriteLine(valid
            ? "Integridade valida. Nenhum dado foi enviado pela rede."
            : "Integridade invalida. O jogo nao deve ser iniciado.");
        return new VerificationResult(valid, manifest);
    }

    private static void ValidateManifest(IntegrityManifest manifest)
    {
        if (manifest.SchemaVersion != 1)
            throw new InvalidOperationException("Versao de manifesto nao suportada.");
        if (!string.Equals(manifest.HashAlgorithm, "SHA-256", StringComparison.OrdinalIgnoreCase))
            throw new InvalidOperationException("Algoritmo de hash nao suportado.");
        if (manifest.Files.Count == 0)
            throw new InvalidOperationException("O manifesto nao contem arquivos.");
    }

    private static bool FixedTimeHexEquals(string actual, string expected)
    {
        if (actual.Length != 64 || expected.Length != 64)
            return false;

        try
        {
            return CryptographicOperations.FixedTimeEquals(
                Convert.FromHexString(actual),
                Convert.FromHexString(expected));
        }
        catch (FormatException)
        {
            return false;
        }
    }

    private static bool IsPortableExecutable(string path)
    {
        try
        {
            using var stream = new FileStream(
                path,
                FileMode.Open,
                FileAccess.Read,
                FileShare.ReadWrite | FileShare.Delete);
            return stream.ReadByte() == 'M' && stream.ReadByte() == 'Z';
        }
        catch (IOException)
        {
            return false;
        }
        catch (UnauthorizedAccessException)
        {
            return false;
        }
    }
}
