using System.Security.Cryptography;

namespace VibeGuard.Manifest;

internal static class ManifestSignature
{
    public static async Task GenerateKeyPairAsync(string privateKeyPath, string publicKeyPath)
    {
        var privatePath = Path.GetFullPath(privateKeyPath);
        var publicPath = Path.GetFullPath(publicKeyPath);
        RefuseOverwrite(privatePath);
        RefuseOverwrite(publicPath);

        Directory.CreateDirectory(Path.GetDirectoryName(privatePath)
            ?? throw new InvalidOperationException("Caminho da chave privada invalido."));
        Directory.CreateDirectory(Path.GetDirectoryName(publicPath)
            ?? throw new InvalidOperationException("Caminho da chave publica invalido."));

        using var key = ECDsa.Create(ECCurve.NamedCurves.nistP256);
        await File.WriteAllTextAsync(privatePath, key.ExportECPrivateKeyPem());
        await File.WriteAllTextAsync(publicPath, key.ExportSubjectPublicKeyInfoPem());

        Console.WriteLine($"Chave privada de laboratorio criada: {privatePath}");
        Console.WriteLine($"Chave publica criada: {publicPath}");
        Console.WriteLine("Nunca distribua ou versione a chave privada.");
    }

    public static async Task SignAsync(
        string manifestPath,
        string privateKeyPath,
        string signaturePath)
    {
        var manifest = await File.ReadAllBytesAsync(Path.GetFullPath(manifestPath));
        var privatePem = await File.ReadAllTextAsync(Path.GetFullPath(privateKeyPath));

        using var key = ECDsa.Create();
        key.ImportFromPem(privatePem);
        EnsureP256(key);

        var signature = key.SignData(
            manifest,
            HashAlgorithmName.SHA256,
            DSASignatureFormat.Rfc3279DerSequence);
        var output = Path.GetFullPath(signaturePath);
        Directory.CreateDirectory(Path.GetDirectoryName(output)
            ?? throw new InvalidOperationException("Caminho da assinatura invalido."));
        await File.WriteAllTextAsync(output, Convert.ToBase64String(signature) + Environment.NewLine);

        Console.WriteLine($"Manifesto assinado: {output}");
    }

    public static async Task VerifyTrustedAsync(string manifestPath, string signaturePath)
    {
        if (string.IsNullOrWhiteSpace(TrustedPublicKey.Pem))
            throw new InvalidOperationException("O VibeGuard nao possui uma chave publica confiavel embutida.");

        var manifest = await File.ReadAllBytesAsync(Path.GetFullPath(manifestPath));
        var encodedSignature = (await File.ReadAllTextAsync(Path.GetFullPath(signaturePath))).Trim();
        byte[] signature;
        try
        {
            signature = Convert.FromBase64String(encodedSignature);
        }
        catch (FormatException)
        {
            throw new InvalidOperationException("Formato de assinatura invalido.");
        }

        using var key = ECDsa.Create();
        key.ImportFromPem(TrustedPublicKey.Pem);
        EnsureP256(key);
        var valid = key.VerifyData(
            manifest,
            signature,
            HashAlgorithmName.SHA256,
            DSASignatureFormat.Rfc3279DerSequence);

        if (!valid)
            throw new InvalidOperationException("Assinatura do manifesto invalida.");

        Console.WriteLine("[OK] Assinatura digital do manifesto");
    }

    private static void EnsureP256(ECDsa key)
    {
        if (key.KeySize != 256)
            throw new InvalidOperationException("Somente chaves ECDSA P-256 sao permitidas.");
    }

    private static void RefuseOverwrite(string path)
    {
        if (File.Exists(path))
            throw new InvalidOperationException($"O arquivo ja existe e nao sera sobrescrito: {path}");
    }
}
