using System.Drawing.Drawing2D;
using System.Text;

namespace VibeGuard.Manifest;

internal sealed class GuardWindow : Form
{
    private readonly Label statusLabel;
    private readonly Label detailLabel;
    private readonly Label bindingLabel;
    private readonly ProgressBar progressBar;
    private readonly Button hideButton;
    private readonly NotifyIcon trayIcon;
    private readonly System.Windows.Forms.Timer autoHideTimer;
    private readonly System.Windows.Forms.Timer startupProgressTimer;
    private readonly System.Windows.Forms.Timer completedProgressHideTimer;
    private bool launchRunning;
    private bool allowClose;
    private string? lastError;

    public int ExitCode { get; private set; } = 1;

    public GuardWindow()
    {
        Text = "VibeGuard";
        ClientSize = new Size(500, 410);
        FormBorderStyle = FormBorderStyle.FixedSingle;
        MaximizeBox = false;
        StartPosition = FormStartPosition.CenterScreen;
        BackColor = Color.FromArgb(13, 20, 31);
        ForeColor = Color.White;
        Font = new Font("Segoe UI", 9F);
        Icon = SystemIcons.Shield;
        TopMost = true;

        var banner = CreateBanner();
        var shield = new ShieldPanel { Location = new Point(24, 167), Size = new Size(58, 66) };
        var title = new Label
        {
            AutoSize = true,
            Font = new Font("Segoe UI Semibold", 19F),
            ForeColor = Color.White,
            Location = new Point(98, 169),
            Text = "VibeGuard"
        };
        var subtitle = new Label
        {
            AutoSize = true,
            ForeColor = Color.FromArgb(151, 165, 184),
            Location = new Point(101, 208),
            Text = "Protecao e integridade do cliente VibeRO"
        };
        statusLabel = new Label
        {
            AutoSize = false,
            Font = new Font("Segoe UI Semibold", 11F),
            ForeColor = Color.FromArgb(89, 220, 151),
            Location = new Point(24, 246),
            Size = new Size(452, 26),
            Text = "Preparando verificacao..."
        };
        detailLabel = new Label
        {
            AutoSize = false,
            ForeColor = Color.FromArgb(181, 193, 209),
            Location = new Point(24, 275),
            Size = new Size(452, 38),
            Text = "Inicializando os componentes de seguranca do cliente."
        };
        progressBar = new ProgressBar
        {
            Location = new Point(24, 317),
            Size = new Size(452, 7),
            Style = ProgressBarStyle.Marquee,
            MarqueeAnimationSpeed = 25
        };
        bindingLabel = new Label
        {
            AutoSize = false,
            Font = new Font("Segoe UI Semibold", 9F),
            ForeColor = Color.FromArgb(110, 231, 183),
            Location = new Point(24, 338),
            Size = new Size(350, 30),
            TextAlign = ContentAlignment.MiddleLeft,
            Text = "Verificacao de seguranca em andamento"
        };
        hideButton = CreateButton("Ocultar", new Point(398, 338), new Size(78, 30));
        hideButton.Enabled = false;
        hideButton.Click += async (_, _) =>
        {
            if (launchRunning)
            {
                HideToTray();
                return;
            }
            hideButton.Enabled = false;
            lastError = null;
            statusLabel.ForeColor = Color.FromArgb(89, 220, 151);
            statusLabel.Text = "Preparando nova tentativa...";
            detailLabel.Text = "Verificando novamente a conexao com o servidor.";
            progressBar.Style = ProgressBarStyle.Marquee;
            await StartGuardAsync();
        };
        var privacyLabel = new Label
        {
            AutoSize = true,
            Font = new Font("Segoe UI", 8F),
            ForeColor = Color.FromArgb(111, 126, 147),
            Location = new Point(24, 382),
            Text = "VibeGuard - integridade verificada - protecao ativa"
        };

        Controls.AddRange([
            banner, shield, title, subtitle, statusLabel, detailLabel,
            progressBar, bindingLabel, hideButton, privacyLabel
        ]);

        trayIcon = new NotifyIcon
        {
            Icon = SystemIcons.Shield,
            Text = "VibeGuard - protecao ativa",
            Visible = false
        };
        trayIcon.DoubleClick += (_, _) => RestoreFromTray();
        autoHideTimer = new System.Windows.Forms.Timer { Interval = 20000 };
        autoHideTimer.Tick += (_, _) =>
        {
            autoHideTimer.Stop();
            HideToTray();
        };
        // Complete the visual animation slightly before the five-second startup
        // delay so normal UI timer jitter cannot hide the window below 100%.
        startupProgressTimer = new System.Windows.Forms.Timer { Interval = 25 };
        startupProgressTimer.Tick += (_, _) =>
        {
            progressBar.Value = Math.Min(100, progressBar.Value + 2);
            if (progressBar.Value >= 100)
                startupProgressTimer.Stop();
        };
        completedProgressHideTimer = new System.Windows.Forms.Timer { Interval = 250 };
        completedProgressHideTimer.Tick += (_, _) =>
        {
            completedProgressHideTimer.Stop();
            HideToTray();
        };
        Shown += async (_, _) => await StartGuardAsync();
        FormClosing += OnFormClosing;
    }

    private Control CreateBanner()
    {
        var path = Path.Combine(AppContext.BaseDirectory, "vibeguard-banner.png");
        if (File.Exists(path))
        {
            return new PictureBox
            {
                Location = new Point(0, 0),
                Size = new Size(500, 150),
                SizeMode = PictureBoxSizeMode.Zoom,
                BackColor = Color.FromArgb(17, 31, 48),
                Image = Image.FromFile(path)
            };
        }
        return new Label
        {
            Location = new Point(0, 0),
            Size = new Size(500, 150),
            BackColor = Color.FromArgb(17, 52, 78),
            ForeColor = Color.White,
            Font = new Font("Segoe UI Semibold", 24F, FontStyle.Italic),
            TextAlign = ContentAlignment.MiddleCenter,
            Text = "VibeRO"
        };
    }

    private static Button CreateButton(string text, Point location, Size size) => new()
    {
        Text = text,
        Location = location,
        Size = size,
        FlatStyle = FlatStyle.Flat,
        BackColor = Color.FromArgb(27, 40, 57),
        ForeColor = Color.White,
        FlatAppearance = { BorderColor = Color.FromArgb(64, 84, 107) }
    };

    private async Task StartGuardAsync()
    {
        if (launchRunning)
            return;
        launchRunning = true;
        Console.SetOut(new GuardUiWriter(HandleOutputLine, isError: false));
        Console.SetError(new GuardUiWriter(HandleOutputLine, isError: true));
        var root = AppContext.BaseDirectory;
        await TrustedUpdaterComponent.RemoveOrRecognizeAsync(
            Path.Combine(root, "PathVibe.Patcher.c688e64d237c702c.exe"));
        LauncherSettings settings;
        try
        {
            settings = LauncherSettings.Load(root);
        }
        catch (Exception exception)
        {
            Console.Error.WriteLine($"Configuracao invalida: {exception.Message}");
            ExitCode = 1;
            launchRunning = false;
            ShowLaunchFailure();
            return;
        }
        var arguments = new[]
        {
            "launch",
            "--game-dir", root,
            "--manifest", Path.Combine(root, "vibeguard-manifest.json"),
            "--signature", Path.Combine(root, "vibeguard-manifest.sig"),
            "--executable", settings.Executable,
            "--server", settings.Server,
            "--allow-insecure-http", settings.AllowInsecureHttp.ToString(),
            "--startup-delay-ms", settings.StartupDelayMilliseconds.ToString(),
            "--require-session", "true"
        };

        ExitCode = await Task.Run(() => CommandRunner.RunAsync(arguments));
        launchRunning = false;
        if (ExitCode == 0)
        {
            allowClose = true;
            trayIcon.Visible = false;
            Close();
            return;
        }
        ShowLaunchFailure();
    }

    private void ShowLaunchFailure()
    {
        RunOnUi(() =>
        {
            RestoreFromTray();
            statusLabel.Text = "Nao foi possivel iniciar com protecao";
            statusLabel.ForeColor = Color.FromArgb(248, 113, 113);
            progressBar.Style = ProgressBarStyle.Continuous;
            progressBar.Value = 0;
            if (!string.IsNullOrWhiteSpace(lastError))
                detailLabel.Text = lastError;
            bindingLabel.Text = "A conexao segura nao foi concluida";
            hideButton.Text = "Tentar novamente";
            hideButton.Enabled = true;
            autoHideTimer.Stop();
        });
    }

    private void HandleOutputLine(string line, bool isError)
    {
        if (string.IsNullOrWhiteSpace(line))
            return;
        if (isError || IsVerificationFailure(line))
            lastError = line;
        RunOnUi(() =>
        {
            if (line.StartsWith("[OK]", StringComparison.Ordinal))
            {
                statusLabel.Text = "Verificando arquivos assinados...";
                detailLabel.Text = line;
            }
            else if (line.StartsWith("Integridade valida", StringComparison.OrdinalIgnoreCase))
            {
                statusLabel.Text = "Integridade confirmada";
                detailLabel.Text = "Arquivos aprovados. Abrindo sessao autenticada.";
            }
            else if (line.StartsWith("Sessao de observacao autenticada", StringComparison.OrdinalIgnoreCase))
            {
                statusLabel.Text = "Sessao autenticada";
                detailLabel.Text = "Conexao segura estabelecida com o servidor.";
            }
            else if (line.StartsWith("Conexao temporariamente indisponivel", StringComparison.OrdinalIgnoreCase))
            {
                statusLabel.Text = "Reconectando ao servidor...";
                detailLabel.Text = line;
            }
            else if (line.StartsWith("Conta vinculada automaticamente apos", StringComparison.OrdinalIgnoreCase))
            {
                statusLabel.Text = "Protecao ativa";
                bindingLabel.Text = "Todos os modulos foram inicializados";
                detailLabel.Text = "O cliente permanece protegido durante esta sessao.";
            }
            else if (line.StartsWith("Carregando protecao:", StringComparison.OrdinalIgnoreCase))
            {
                statusLabel.Text = "Carregando protecao...";
                detailLabel.Text = "Inicializando os modulos de seguranca do cliente.";
                progressBar.Style = ProgressBarStyle.Continuous;
                progressBar.Value = 0;
                startupProgressTimer.Start();
            }
            else if (line.StartsWith("Interface pronta para ocultar", StringComparison.OrdinalIgnoreCase))
            {
                startupProgressTimer.Stop();
                progressBar.Style = ProgressBarStyle.Continuous;
                progressBar.Value = 100;
                statusLabel.Text = "Protecao carregada";
                progressBar.Refresh();
                completedProgressHideTimer.Start();
            }
            else if (line.StartsWith("Iniciando arquivo aprovado", StringComparison.OrdinalIgnoreCase))
            {
                statusLabel.Text = "Protecao ativa - iniciando VibeRO";
                detailLabel.Text = "Concluindo a inicializacao segura do cliente.";
                progressBar.Style = ProgressBarStyle.Continuous;
                progressBar.Value = 100;
            }
            else if (isError || IsVerificationFailure(line))
            {
                statusLabel.Text = "Verificacao requer atencao";
                statusLabel.ForeColor = Color.FromArgb(248, 113, 113);
                detailLabel.Text = line;
            }
        });
    }

    private static bool IsVerificationFailure(string line) =>
        line.StartsWith("[ALTERADO]", StringComparison.Ordinal)
        || line.StartsWith("[AUSENTE]", StringComparison.Ordinal)
        || line.StartsWith("[NAO APROVADO]", StringComparison.Ordinal);

    private void HideToTray()
    {
        if (!launchRunning)
        {
            allowClose = true;
            Close();
            return;
        }
        trayIcon.Visible = true;
        Hide();
    }

    private void RestoreFromTray()
    {
        Show();
        WindowState = FormWindowState.Normal;
        Activate();
        trayIcon.Visible = false;
    }

    private void OnFormClosing(object? sender, FormClosingEventArgs eventArgs)
    {
        if (launchRunning && !allowClose)
        {
            eventArgs.Cancel = true;
            HideToTray();
        }
    }

    private void RunOnUi(Action action)
    {
        if (IsDisposed)
            return;
        if (InvokeRequired)
            BeginInvoke(action);
        else
            action();
    }

    protected override void Dispose(bool disposing)
    {
        if (disposing)
        {
            autoHideTimer.Dispose();
            startupProgressTimer.Dispose();
            completedProgressHideTimer.Dispose();
            trayIcon.Dispose();
        }
        base.Dispose(disposing);
    }

    private sealed class GuardUiWriter(Action<string, bool> output, bool isError) : TextWriter
    {
        private readonly StringBuilder buffer = new();
        public override Encoding Encoding => Encoding.UTF8;
        public override void Write(char value)
        {
            if (value == '\n')
            {
                output(buffer.ToString().TrimEnd('\r'), isError);
                buffer.Clear();
            }
            else
                buffer.Append(value);
        }
        public override void WriteLine(string? value) => output(value ?? string.Empty, isError);
    }

    private sealed class ShieldPanel : Panel
    {
        public ShieldPanel() => DoubleBuffered = true;
        protected override void OnPaint(PaintEventArgs eventArgs)
        {
            base.OnPaint(eventArgs);
            eventArgs.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
            var points = new[]
            {
                new PointF(Width / 2F, 3), new PointF(Width - 7, 12),
                new PointF(Width - 10, 45), new PointF(Width / 2F, Height - 4),
                new PointF(10, 45), new PointF(7, 12)
            };
            using var fill = new SolidBrush(Color.FromArgb(24, 170, 112));
            using var border = new Pen(Color.FromArgb(110, 231, 183), 2F);
            eventArgs.Graphics.FillPolygon(fill, points);
            eventArgs.Graphics.DrawPolygon(border, points);
            using var font = new Font("Segoe UI Semibold", 14F, FontStyle.Bold);
            using var text = new SolidBrush(Color.White);
            var value = "VG";
            var size = eventArgs.Graphics.MeasureString(value, font);
            eventArgs.Graphics.DrawString(value, font, text, (Width - size.Width) / 2F, 21F);
        }
    }
}
