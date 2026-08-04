namespace VibeGuard.Manifest;

internal static class TrustedUpdaterComponent
{
    private const string LegacyPatcherName = "PathVibe.Patcher.c688e64d237c702c.exe";
    private const long LegacyPatcherLength = 3_542_528;
    private const string LegacyPatcherSha256 = "c688e64d237c702c09eaa247cc59138969d06d5facc036564378a31b3ecb1d37";

    public static async Task<bool> RemoveOrRecognizeAsync(string path)
    {
        if (!await IsTrustedAsync(path))
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
            return await IsTrustedAsync(path);
        }
    }

    private static async Task<bool> IsTrustedAsync(string path)
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
