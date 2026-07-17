namespace VibeGuard.Manifest;

internal static class TrustedPublicKey
{
    // Somente a chave publica e distribuida. A chave privada fica fora do Git e do cliente.
    public const string Pem = """
        -----BEGIN PUBLIC KEY-----
        MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEKzZ5OPBgbvRVjxO+RPM1THFF7EFh
        l1frRc02URxxQ8JkwKxupfOGddrPtdpsvOaVid+RLGlLWNSjttls4cjr9g==
        -----END PUBLIC KEY-----
        """;
}
