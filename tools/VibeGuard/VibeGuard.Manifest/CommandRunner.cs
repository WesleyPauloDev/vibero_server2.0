using System.ComponentModel;
using System.Diagnostics;

namespace VibeGuard.Manifest;

internal static class CommandRunner
{
    public static async Task<int> RunAsync(string[] args)
    {
        try
        {
            if (args.Length == 0 || args[0] is "--help" or "-h")
            {
                PrintHelp();
                return args.Length == 0 ? 1 : 0;
            }

            var command = args[0].ToLowerInvariant();
            var options = ParseOptions(args[1..]);

            switch (command)
            {
                case "generate":
                    RequireOnly(options, "--game-dir", "--policy", "--output");
                    await ManifestService.GenerateAsync(
                        GetRequired(options, "--game-dir"),
                        GetRequired(options, "--policy"),
                        GetRequired(options, "--output"));
                    return 0;

                case "verify":
                    RequireOnly(options, "--game-dir", "--manifest", "--signature");
                    var verification = await ManifestService.VerifyAsync(
                        GetRequired(options, "--game-dir"),
                        GetRequired(options, "--manifest"),
                        GetRequired(options, "--signature"));
                    return verification.IsValid ? 0 : 2;

                case "launch":
                    RequireOnlyWithOptional(
                        options,
                        ["--game-dir", "--manifest", "--signature", "--executable", "--server"],
                        ["--account", "--startup-delay-ms", "--require-session", "--allow-insecure-http"]);
                    return await LaunchAsync(options);

                case "keygen":
                    RequireOnly(options, "--private-key", "--public-key");
                    await ManifestSignature.GenerateKeyPairAsync(
                        GetRequired(options, "--private-key"),
                        GetRequired(options, "--public-key"));
                    return 0;

                case "sign":
                    RequireOnly(options, "--manifest", "--private-key", "--signature");
                    await ManifestSignature.SignAsync(
                        GetRequired(options, "--manifest"),
                        GetRequired(options, "--private-key"),
                        GetRequired(options, "--signature"));
                    return 0;

                case "session-test":
                    RequireOnlyWithOptional(
                        options,
                        ["--server", "--manifest", "--signature"],
                        ["--account"]);
                    await ManifestSignature.VerifyTrustedAsync(
                        GetRequired(options, "--manifest"),
                        GetRequired(options, "--signature"));
                    await using (var session = await ObservationSessionClient.TryOpenAsync(
                        GetRequired(options, "--server"),
                        GetRequired(options, "--manifest")))
                    {
                        if (session is null)
                            return 4;
                        if (options.TryGetValue("--account", out var testAccount)
                            && !await session.ClaimAccountAsync(testAccount))
                            return 4;
                        return await session.SendDiagnosticHeartbeatAsync() ? 0 : 4;
                    }

                default:
                    throw new ArgumentException($"Comando desconhecido: {command}");
            }
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine($"Erro: {exception.Message}");
            return 1;
        }
    }

    private static async Task<int> LaunchAsync(Dictionary<string, string> options)
    {
        var gameDirectory = GetRequired(options, "--game-dir");
        var executable = SafeGamePath.NormalizeRelative(GetRequired(options, "--executable"));
        var verification = await ManifestService.VerifyAsync(
            gameDirectory,
            GetRequired(options, "--manifest"),
            GetRequired(options, "--signature"));

        if (!verification.IsValid)
            return 2;

        if (!verification.Manifest.Files.Any(entry =>
                string.Equals(
                    SafeGamePath.NormalizeRelative(entry.Path),
                    executable,
                    StringComparison.OrdinalIgnoreCase)))
            throw new InvalidOperationException("O executavel nao esta protegido pelo manifesto.");

        var root = SafeGamePath.NormalizeRoot(gameDirectory);
        var executablePath = SafeGamePath.ResolveFile(root, executable);
        var existingGameProcessIds = Process.GetProcessesByName("Ragexe")
            .Select(process =>
            {
                using (process)
                    return process.Id;
            })
            .ToHashSet();
        await using var observation = await ObservationSessionClient.TryOpenAsync(
            GetRequired(options, "--server"),
            GetRequired(options, "--manifest"),
            GetOptionalBoolean(options, "--allow-insecure-http"));
        if (options.TryGetValue("--require-session", out var requireSessionText)
            && bool.TryParse(requireSessionText, out var requireSession)
            && requireSession
            && observation is null)
            return 6;
        if (options.TryGetValue("--account", out var accountName)
            && (observation is null || !await observation.ClaimAccountAsync(accountName)))
            return 4;

        if (options.TryGetValue("--startup-delay-ms", out var delayText))
        {
            if (!int.TryParse(delayText, out var delayMilliseconds)
                || delayMilliseconds is < 0 or > 15000)
                throw new ArgumentException("O atraso de inicializacao deve estar entre 0 e 15000 ms.");
            if (delayMilliseconds > 0)
            {
                Console.WriteLine($"Carregando protecao: {delayMilliseconds}");
                await Task.Delay(delayMilliseconds);
                Console.WriteLine("Interface pronta para ocultar.");
                await Task.Delay(350);
            }
        }
        Console.WriteLine($"Iniciando arquivo aprovado: {executable.Replace('\\', '/')}");

        Process? process;
        try
        {
            process = StartProcess(executablePath, root, elevate: false);
        }
        catch (Win32Exception exception) when (exception.NativeErrorCode == 740)
        {
            Console.WriteLine("O executavel exige permissao de administrador. Solicitando UAC...");
            try
            {
                process = StartProcess(executablePath, root, elevate: true);
            }
            catch (Win32Exception cancelled) when (cancelled.NativeErrorCode == 1223)
            {
                Console.Error.WriteLine("A elevacao foi cancelada pelo usuario.");
                return 3;
            }
        }

        if (observation is not null)
        {
            var monitor = new RuntimeIntegrityMonitor(root, verification.Manifest);
            if (!await observation.ObserveProcessAsync(process, existingGameProcessIds, monitor))
                return 7;
        }

        return 0;
    }

    private static Process StartProcess(string executablePath, string workingDirectory, bool elevate)
    {
        return Process.Start(new ProcessStartInfo
        {
            FileName = executablePath,
            WorkingDirectory = workingDirectory,
            UseShellExecute = elevate,
            Verb = elevate ? "runas" : string.Empty
        }) ?? throw new InvalidOperationException("Nao foi possivel iniciar o jogo.");
    }

    private static Dictionary<string, string> ParseOptions(string[] args)
    {
        if (args.Length % 2 != 0)
            throw new ArgumentException("Cada opcao precisa de um valor.");

        var options = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        for (var index = 0; index < args.Length; index += 2)
        {
            var name = args[index];
            if (!name.StartsWith("--", StringComparison.Ordinal) || !options.TryAdd(name, args[index + 1]))
                throw new ArgumentException($"Opcao invalida ou repetida: {name}");
        }
        return options;
    }

    private static string GetRequired(Dictionary<string, string> options, string name) =>
        options.TryGetValue(name, out var value) && !string.IsNullOrWhiteSpace(value)
            ? value
            : throw new ArgumentException($"Opcao obrigatoria ausente: {name}");

    private static bool GetOptionalBoolean(Dictionary<string, string> options, string name)
    {
        if (!options.TryGetValue(name, out var text))
            return false;
        if (!bool.TryParse(text, out var value))
            throw new ArgumentException($"A opcao {name} deve ser true ou false.");
        return value;
    }

    private static void RequireOnly(Dictionary<string, string> options, params string[] allowed)
    {
        var allowedSet = new HashSet<string>(allowed, StringComparer.OrdinalIgnoreCase);
        var unexpected = options.Keys.FirstOrDefault(key => !allowedSet.Contains(key));
        if (unexpected is not null)
            throw new ArgumentException($"Opcao nao permitida neste comando: {unexpected}");
        foreach (var name in allowed)
            GetRequired(options, name);
    }

    private static void RequireOnlyWithOptional(
        Dictionary<string, string> options,
        IReadOnlyCollection<string> required,
        IReadOnlyCollection<string> optional)
    {
        var allowedSet = new HashSet<string>(required, StringComparer.OrdinalIgnoreCase);
        allowedSet.UnionWith(optional);
        var unexpected = options.Keys.FirstOrDefault(key => !allowedSet.Contains(key));
        if (unexpected is not null)
            throw new ArgumentException($"Opcao nao permitida neste comando: {unexpected}");
        foreach (var name in required)
            GetRequired(options, name);
    }

    private static void PrintHelp()
    {
        Console.WriteLine("VibeGuard - integridade local do cliente");
        Console.WriteLine();
        Console.WriteLine("generate --game-dir <pasta> --policy <politica> --output <manifesto>");
        Console.WriteLine("keygen   --private-key <privada.pem> --public-key <publica.pem>");
        Console.WriteLine("sign     --manifest <manifesto> --private-key <privada.pem> --signature <assinatura>");
        Console.WriteLine("session-test --server <url-loopback> --manifest <manifesto> --signature <assinatura>");
        Console.WriteLine("verify   --game-dir <pasta> --manifest <manifesto> --signature <assinatura>");
        Console.WriteLine("launch   --game-dir <pasta> --manifest <manifesto> --signature <assinatura> --executable <arquivo> --server <url-loopback> [--account <conta>]");
    }
}
