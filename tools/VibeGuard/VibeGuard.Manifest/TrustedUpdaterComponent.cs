namespace VibeGuard.Manifest;

internal static class TrustedUpdaterComponent
{
    private const string LegacyPatcherName = "PathVibe.Patcher.c688e64d237c702c.exe";
    private const long LegacyPatcherLength = 3_542_528;
    private const string LegacyPatcherSha256 = "c688e64d237c702c09eaa247cc59138969d06d5facc036564378a31b3ecb1d37";
    private const string UpdaterName = "Atualizador.exe";

    private static readonly (long Length, string Sha256)[] TrustedUpdaterVersions =
    [
        (3_557_888, "dfacd679b823a5794c0c515f89410fa73550e209b1ab019f792f2f3d74ca4d65"),
        (3_642_880, "ac58b76a095235e314ea1b121ca438241b9b3f8952a66737f0ff8b67188cce93")
    ];

    public static async Task<bool> RemoveOrRecognizeAsync(string path)
    {
        if (await IsTrustedUpdaterAsync(path))
            return true;

        if (!await IsLegacyPatcherAsync(path))
            return false;

        try
        {
            File.SetAttributes(path, FileAttributes.Normal);
            File.Delete(path);
            return true;
        }
        catch (Exception exception) when (exception is IOException or UnauthorizedAccessException)
        {
            // The official patcher can still be executing while it starts VibeGuard.
            // Recheck the full hash so a replaced file is never accepted by name alone.
            return await IsLegacyPatcherAsync(path);
        }
    }

    private static async Task<bool> IsTrustedUpdaterAsync(string path)
    {
        try
        {
            var info = new FileInfo(path);
            if (!info.Exists || !string.Equals(info.Name, UpdaterName, StringComparison.OrdinalIgnoreCase))
                return false;

            var version = TrustedUpdaterVersions.FirstOrDefault(candidate => candidate.Length == info.Length);
            if (version == default)
                return false;

            var actualHash = await FileHash.Sha256Async(path);
            return string.Equals(actualHash, version.Sha256, StringComparison.OrdinalIgnoreCase);
        }
        catch (Exception exception) when (exception is IOException or UnauthorizedAccessException)
        {
            return false;
        }
    }

    private static async Task<bool> IsLegacyPatcherAsync(string path)
    {
        try
        {
            var info = new FileInfo(path);
            if (!info.Exists
                || info.Length != LegacyPatcherLength
                || !string.Equals(info.Name, LegacyPatcherName, StringComparison.OrdinalIgnoreCase))
                return false;

            var actualHash = await FileHash.Sha256Async(path);
            return string.Equals(actualHash, LegacyPatcherSha256, StringComparison.OrdinalIgnoreCase);
        }
        catch (Exception exception) when (exception is IOException or UnauthorizedAccessException)
        {
            return false;
        }
    }
}
