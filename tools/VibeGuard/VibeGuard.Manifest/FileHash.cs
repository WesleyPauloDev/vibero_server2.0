using System.Security.Cryptography;

namespace VibeGuard.Manifest;

internal static class FileHash
{
    public static async Task<string> Sha256Async(string path)
    {
        await using var stream = new FileStream(
            path, FileMode.Open, FileAccess.Read, FileShare.Read,
            bufferSize: 1024 * 1024,
            FileOptions.Asynchronous | FileOptions.SequentialScan);
        var digest = await SHA256.HashDataAsync(stream);
        return Convert.ToHexString(digest).ToLowerInvariant();
    }
}
