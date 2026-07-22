using System.Diagnostics;
using System.Net.Http.Headers;
using System.Net.Http.Json;
using System.Security.Cryptography;
using System.Text.Json;

namespace VibeGuard.Manifest;

internal sealed class ObservationSessionClient : IAsyncDisposable
{
    private const string ClientVersion = "1.0.0";
    private const int OpenAttempts = 3;
    private readonly HttpClient client;
    private readonly string token;
    private readonly int heartbeatSeconds;
    private bool closed;

    public string SessionId { get; }

    private ObservationSessionClient(
        HttpClient client,
        string sessionId,
        string token,
        int heartbeatSeconds)
    {
        this.client = client;
        this.token = token;
        this.heartbeatSeconds = heartbeatSeconds;
        SessionId = sessionId;
    }

    public static async Task<ObservationSessionClient?> TryOpenAsync(
        string serverAddress,
        string manifestPath,
        bool allowInsecureHttp = false)
    {
        try
        {
            var baseAddress = NormalizeServerAddress(serverAddress, allowInsecureHttp);
            var handler = new SocketsHttpHandler
            {
                UseProxy = false,
                AllowAutoRedirect = false
            };
            var client = new HttpClient(handler)
            {
                BaseAddress = baseAddress,
                Timeout = TimeSpan.FromSeconds(8)
            };

            var manifestBytes = await File.ReadAllBytesAsync(Path.GetFullPath(manifestPath));
            var manifestHash = Convert.ToHexString(SHA256.HashData(manifestBytes)).ToLowerInvariant();
            var requestBody = new
            {
                protocolVersion = 1,
                clientVersion = ClientVersion,
                manifestSha256 = manifestHash
            };
            HttpResponseMessage? openResponse = null;
            for (var attempt = 1; attempt <= OpenAttempts; ++attempt)
            {
                try
                {
                    openResponse = await client.PostAsJsonAsync(
                        "vibeguard/v1/session/open",
                        requestBody,
                        JsonSupport.Options);
                    var statusCode = (int)openResponse.StatusCode;
                    if (openResponse.IsSuccessStatusCode
                        || (statusCode < 500 && statusCode is not 408 and not 429))
                        break;
                    if (attempt < OpenAttempts)
                    {
                        openResponse.Dispose();
                        openResponse = null;
                        Console.WriteLine($"Conexao temporariamente indisponivel. Nova tentativa {attempt + 1}/{OpenAttempts}...");
                        await Task.Delay(TimeSpan.FromSeconds(attempt));
                    }
                }
                catch (Exception exception) when (
                    attempt < OpenAttempts
                    && (exception is HttpRequestException or TaskCanceledException))
                {
                    Console.WriteLine($"Conexao temporariamente indisponivel. Nova tentativa {attempt + 1}/{OpenAttempts}...");
                    await Task.Delay(TimeSpan.FromSeconds(attempt));
                }
            }
            using var response = openResponse
                ?? throw new HttpRequestException("nao houve resposta do servidor apos tres tentativas");

            if (!response.IsSuccessStatusCode)
                throw new InvalidOperationException($"servidor retornou HTTP {(int)response.StatusCode}");

            var session = await response.Content.ReadFromJsonAsync<OpenSessionResponse>(JsonSupport.Options)
                ?? throw new InvalidOperationException("resposta de sessao vazia");
            ValidateResponse(session);

            Console.WriteLine($"Sessao de observacao autenticada: {session.SessionId}");
            Console.WriteLine($"Vincule no jogo com: @guard {session.PairingCode}");
            Console.WriteLine("Aguardando o login para vincular a conta automaticamente.");
            Console.WriteLine("Nenhuma senha, IP, hardware ID ou lista de processos foi enviada.");
            return new ObservationSessionClient(
                client,
                session.SessionId!,
                session.Token!,
                session.HeartbeatSeconds);
        }
        catch (Exception exception) when (
            exception is HttpRequestException
            or TaskCanceledException
            or IOException
            or InvalidOperationException
            or JsonException)
        {
            Console.Error.WriteLine($"Observacao indisponivel: {exception.Message}");
            return null;
        }
    }

    public async Task<bool> ObserveProcessAsync(
        Process launcherProcess,
        IReadOnlySet<int> ignoredGameProcessIds,
        RuntimeIntegrityMonitor integrityMonitor)
    {
        ulong sequence = 1;
        var accountBound = false;
        try
        {
            while (!launcherProcess.HasExited)
            {
                var integrity = await integrityMonitor.VerifyAsync(launcherProcess);
                if (!integrity.Valid)
                {
                    Console.Error.WriteLine($"Integridade em execucao invalida: {integrity.Reason}");
                    return false;
                }
                _ = await SendHeartbeatAsync(sequence++);
                if (!accountBound)
                    accountBound = await TryAutoClaimAsync();
                await Task.Delay(TimeSpan.FromSeconds(heartbeatSeconds));
            }

            var gameProcess = await FindNewGameProcessAsync(ignoredGameProcessIds);
            if (gameProcess is null)
                return true;

            Console.WriteLine("Processo Ragexe iniciado pelo launcher detectado localmente.");
            using (gameProcess)
            {
                while (!gameProcess.HasExited)
                {
                    var integrity = await integrityMonitor.VerifyAsync(gameProcess);
                    if (!integrity.Valid)
                    {
                        Console.Error.WriteLine($"Integridade em execucao invalida: {integrity.Reason}");
                        return false;
                    }
                    _ = await SendHeartbeatAsync(sequence++);
                    if (!accountBound)
                        accountBound = await TryAutoClaimAsync();
                    await Task.Delay(TimeSpan.FromSeconds(heartbeatSeconds));
                }
            }
        }
        catch (InvalidOperationException)
        {
            // The elevated process handle may become unavailable during shutdown.
        }
        finally
        {
            await CloseAsync();
        }
        return true;
    }

    private static async Task<Process?> FindNewGameProcessAsync(
        IReadOnlySet<int> ignoredProcessIds)
    {
        var deadline = DateTime.UtcNow.AddSeconds(30);
        while (DateTime.UtcNow < deadline)
        {
            foreach (var candidate in Process.GetProcessesByName("Ragexe"))
            {
                if (!ignoredProcessIds.Contains(candidate.Id))
                    return candidate;
                candidate.Dispose();
            }

            await Task.Delay(TimeSpan.FromSeconds(2));
        }
        return null;
    }

    public Task<bool> SendDiagnosticHeartbeatAsync() => SendHeartbeatAsync(1);

    public async Task<bool> ClaimAccountAsync(string accountName)
    {
        try
        {
            using var request = CreateAuthenticatedRequest(
                HttpMethod.Post,
                "vibeguard/v1/session/claim");
            request.Content = JsonContent.Create(
                new { accountName },
                options: JsonSupport.Options);
            using var response = await client.SendAsync(request);
            if (!response.IsSuccessStatusCode)
            {
                Console.Error.WriteLine(response.StatusCode == System.Net.HttpStatusCode.NotFound
                    ? "Conta nao encontrada. Confira o nome usado no login."
                    : $"Vinculo automatico recusado: HTTP {(int)response.StatusCode}.");
                return false;
            }

            var result = await response.Content.ReadFromJsonAsync<ClaimAccountResponse>(JsonSupport.Options);
            if (result is null || !result.Ok || !result.AccountBound || !result.ObservationMode)
            {
                Console.Error.WriteLine("Resposta de vinculo automatico invalida.");
                return false;
            }

            Console.WriteLine($"Conta vinculada automaticamente: {accountName}");
            Console.WriteLine("Somente o nome da conta foi enviado; nenhuma senha foi solicitada.");
            return true;
        }
        catch (Exception exception) when (exception is HttpRequestException or TaskCanceledException or JsonException)
        {
            Console.Error.WriteLine($"Vinculo automatico indisponivel: {exception.Message}");
            return false;
        }
    }

    private async Task<bool> TryAutoClaimAsync()
    {
        try
        {
            using var request = CreateAuthenticatedRequest(
                HttpMethod.Post,
                "vibeguard/v1/session/auto-claim");
            request.Content = JsonContent.Create(new { }, options: JsonSupport.Options);
            using var response = await client.SendAsync(request);
            if (!response.IsSuccessStatusCode)
                return false;
            var result = await response.Content.ReadFromJsonAsync<ClaimAccountResponse>(JsonSupport.Options);
            if (result is null || !result.Ok || !result.ObservationMode)
                return false;
            if (result.AccountBound)
            {
                Console.WriteLine("Conta vinculada automaticamente apos o login.");
                return true;
            }
            return false;
        }
        catch (Exception exception) when (exception is HttpRequestException or TaskCanceledException or JsonException)
        {
            return false;
        }
    }

    public async Task CloseAsync()
    {
        if (closed)
            return;
        closed = true;

        try
        {
            using var request = CreateAuthenticatedRequest(
                HttpMethod.Post,
                "vibeguard/v1/session/close");
            using var response = await client.SendAsync(request);
            if (response.IsSuccessStatusCode)
                Console.WriteLine($"Sessao de observacao encerrada: {SessionId}");
        }
        catch (Exception exception) when (exception is HttpRequestException or TaskCanceledException)
        {
            Console.Error.WriteLine("Nao foi possivel confirmar o encerramento da sessao de observacao.");
        }
    }

    public async ValueTask DisposeAsync()
    {
        await CloseAsync();
        client.Dispose();
    }

    private async Task<bool> SendHeartbeatAsync(ulong sequence)
    {
        try
        {
            using var request = CreateAuthenticatedRequest(
                HttpMethod.Post,
                "vibeguard/v1/session/heartbeat");
            request.Content = JsonContent.Create(
                new { sequence },
                options: JsonSupport.Options);
            using var response = await client.SendAsync(request);
            if (!response.IsSuccessStatusCode)
            {
                Console.Error.WriteLine($"Heartbeat de observacao recusado: HTTP {(int)response.StatusCode}.");
                return false;
            }
            return true;
        }
        catch (Exception exception) when (exception is HttpRequestException or TaskCanceledException)
        {
            Console.Error.WriteLine("Heartbeat de observacao temporariamente indisponivel.");
            return false;
        }
    }

    private HttpRequestMessage CreateAuthenticatedRequest(HttpMethod method, string path)
    {
        var request = new HttpRequestMessage(method, path);
        request.Headers.Authorization = new AuthenticationHeaderValue("Bearer", token);
        return request;
    }

    private static Uri NormalizeServerAddress(string serverAddress, bool allowInsecureHttp)
    {
        if (!Uri.TryCreate(serverAddress, UriKind.Absolute, out var address)
            || (address.Scheme != Uri.UriSchemeHttps && address.Scheme != Uri.UriSchemeHttp)
            || !string.IsNullOrEmpty(address.UserInfo)
            || !string.IsNullOrEmpty(address.Query)
            || !string.IsNullOrEmpty(address.Fragment))
            throw new InvalidOperationException("o endereco do servidor VibeGuard e invalido");

        if (address.Scheme == Uri.UriSchemeHttp && !address.IsLoopback && !allowInsecureHttp)
            throw new InvalidOperationException("servidores remotos exigem HTTPS");

        return new Uri(address.ToString().TrimEnd('/') + "/", UriKind.Absolute);
    }

    private static void ValidateResponse(OpenSessionResponse session)
    {
        if (!session.Ok
            || !session.ObservationMode
            || session.ProtocolVersion != 1
            || session.SessionId is null
            || session.Token is null
            || session.PairingCode is null
            || !IsLowerHex(session.SessionId, 32)
            || !IsLowerHex(session.Token, 64)
            || !IsLowerHex(session.PairingCode, 12)
            || session.HeartbeatSeconds is < 2 or > 60)
            throw new InvalidOperationException("resposta de sessao invalida");
    }

    private static bool IsLowerHex(string value, int length) =>
        value.Length == length && value.All(character =>
            character is >= '0' and <= '9' or >= 'a' and <= 'f');

    private sealed record OpenSessionResponse(
        bool Ok,
        int ProtocolVersion,
        string? SessionId,
        string? Token,
        string? PairingCode,
        int HeartbeatSeconds,
        int ExpiresSeconds,
        bool ObservationMode);

    private sealed record ClaimAccountResponse(
        bool Ok,
        bool AccountBound,
        bool ObservationMode);
}
