namespace VibeGuard.Manifest;

internal sealed record GuardPolicy(
    string Product,
    List<string> Files,
    bool RejectUnexpectedPortableExecutables = false);

internal sealed record ManifestEntry(string Path, long Length, string Sha256);

internal sealed record IntegrityManifest(
    int SchemaVersion,
    string Product,
    string HashAlgorithm,
    List<ManifestEntry> Files,
    bool RejectUnexpectedPortableExecutables = false);

internal sealed record VerificationResult(bool IsValid, IntegrityManifest Manifest);
