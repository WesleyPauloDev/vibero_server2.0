using System.Diagnostics;
using System.Runtime.InteropServices;

namespace VibeTools;

internal static class Program
{
    [STAThread]
    private static void Main()
    {
        ApplicationConfiguration.Initialize();
        Application.Run(new MainWindow());
    }
}

internal sealed class MainWindow : Form
{
    // Launchers such as start.exe and VibeRO.exe must not appear here: their
    // windows do not receive the in-game keyboard input.
    private static readonly string[] ClientNames = ["Ragexe", "VibeRO-Client", "VibeRO_Local"];
    private readonly ComboBox clients = new() { DropDownStyle = ComboBoxStyle.DropDownList };
    private readonly NumericUpDown delay = new() { Minimum = 50, Maximum = 1000, Value = 100, Increment = 10 };
    private readonly Dictionary<int, CheckBox> keyOptions = [];
    private readonly Button toggle = new() { Text = "LIGAR", BackColor = Color.FromArgb(22, 163, 74), ForeColor = Color.White, FlatStyle = FlatStyle.Flat };
    private readonly Label state = new() { Text = "Desligado", ForeColor = Color.FromArgb(220, 38, 38), AutoSize = true };
    private readonly Label targetStatus = new() { Text = "Selecione um cliente aberto.", ForeColor = Color.DimGray, AutoSize = true };
    private readonly System.Windows.Forms.Timer spamTimer = new() { Interval = 10 };
    private readonly System.Windows.Forms.Timer clientTimer = new() { Interval = 2000 };
    private readonly GroupBox clientBox;
    private readonly GroupBox spamBox;
    private readonly KeyboardHook keyboardHook;
    private bool enabled;
    private readonly Dictionary<int, long> nextRepeat = [];
    private readonly Dictionary<int, long> pendingRelease = [];
    private readonly Dictionary<int, long> pendingClicks = [];
    private long mouseReleaseDue;

    internal MainWindow()
    {
        Text = "VibeTools V1";
        ClientSize = new Size(720, 500);
        FormBorderStyle = FormBorderStyle.FixedSingle;
        MaximizeBox = false;
        StartPosition = FormStartPosition.CenterScreen;
        Font = new Font("Segoe UI", 9F);
        BackColor = Color.FromArgb(245, 247, 250);

        var header = new Panel { Dock = DockStyle.Top, Height = 68, BackColor = Color.FromArgb(28, 39, 58) };
        header.Controls.Add(new Label
        {
            Text = "VibeTools V1",
            ForeColor = Color.White,
            Font = new Font("Segoe UI Semibold", 19F),
            Location = new Point(20, 8),
            AutoSize = true
        });
        header.Controls.Add(new Label
        {
            Text = "Ferramentas oficiais de qualidade de vida do VibeRO",
            ForeColor = Color.FromArgb(180, 193, 211),
            Location = new Point(23, 42),
            AutoSize = true
        });
        Controls.Add(header);

        clientBox = Group("Cliente Ragnarok", new Point(18, 82), new Size(684, 92));
        clients.SetBounds(16, 27, 585, 28);
        var refresh = new Button { Text = "Atualizar", Location = new Point(607, 26), Size = new Size(65, 28) };
        refresh.Click += (_, _) => RefreshClients();
        targetStatus.Location = new Point(17, 62);
        clientBox.Controls.AddRange([clients, refresh, targetStatus]);

        spamBox = Group("Teclas do Skill Spammer", new Point(18, 185), new Size(500, 295));
        AddKeyRows(spamBox);
        spamBox.Controls.Add(new Label
        {
            Text = "Ligado: segure uma tecla marcada para repetir somente ela.",
            ForeColor = Color.DimGray,
            Location = new Point(16, 258),
            AutoSize = true
        });

        var controlBox = Group("ON / OFF", new Point(530, 185), new Size(172, 295));
        toggle.SetBounds(31, 32, 136, 42);
        toggle.Font = new Font("Segoe UI Semibold", 12F);
        toggle.Click += (_, _) => SetEnabled(!enabled);
        state.Location = new Point(62, 91);
        controlBox.Controls.AddRange([toggle, state]);
        var legend = new GroupBox { Text = "Legenda", Location = new Point(12, 120), Size = new Size(148, 106) };
        legend.Controls.AddRange([
            Legend(CheckState.Checked, "Com clique", 22),
            Legend(CheckState.Indeterminate, "Sem clique", 49),
            Legend(CheckState.Unchecked, "Desativada", 76)
        ]);
        controlBox.Controls.Add(legend);
        AddField(controlBox, "Intervalo", delay, 238);
        delay.SetBounds(72, 236, 62, 28);
        controlBox.Controls.Add(new Label { Text = "ms", Location = new Point(137, 241), AutoSize = true });
        controlBox.Controls.Add(new Label
        {
            Text = "Desligue para alterar a configuracao.",
            TextAlign = ContentAlignment.MiddleCenter,
            ForeColor = Color.DimGray,
            Location = new Point(10, 267),
            Size = new Size(152, 20)
        });
        Controls.AddRange([clientBox, spamBox, controlBox]);

        clients.SelectedIndexChanged += (_, _) => ReleaseAllKeys();
        spamTimer.Tick += (_, _) => RepeatIfDue();
        spamTimer.Start();
        clientTimer.Tick += (_, _) => RefreshClients();
        clientTimer.Start();
        keyboardHook = new KeyboardHook(OnPhysicalKey);
        RefreshClients();
    }

    private static GroupBox Group(string text, Point location, Size size) => new() { Text = text, Location = location, Size = size };

    private static void AddField(Control parent, string label, Control input, int y)
    {
        parent.Controls.Add(new Label { Text = label, Location = new Point(16, y + 4), AutoSize = true });
        input.SetBounds(142, y, 136, 28);
        parent.Controls.Add(input);
    }

    private void AddKeyRows(Control parent)
    {
        (string Label, int Key)[][] rows =
        [
            Enumerable.Range(1, 9).Select(number => ($"F{number}", 0x6F + number)).ToArray(),
            Enumerable.Range(1, 9).Select(number => (number.ToString(), 0x30 + number)).ToArray(),
            "QWERTYUIO".Select(character => (character.ToString(), (int)character)).ToArray(),
            "ASDFGHJKL".Select(character => (character.ToString(), (int)character)).ToArray(),
            "ZXCVBNM".Select(character => (character.ToString(), (int)character)).ToArray()
        ];
        for (var row = 0; row < rows.Length; row++)
        {
            for (var column = 0; column < rows[row].Length; column++)
            {
                var option = new CheckBox
                {
                    Text = rows[row][column].Label,
                    ThreeState = true,
                    AutoCheck = false,
                    CheckState = CheckState.Unchecked,
                    Location = new Point(16 + column * 52, 28 + row * 40),
                    Size = new Size(50, 25)
                };
                option.Click += (_, _) => Cycle(option);
                keyOptions.Add(rows[row][column].Key, option);
                parent.Controls.Add(option);
            }
        }
    }

    private static void Cycle(CheckBox option) => option.CheckState = option.CheckState switch
    {
        CheckState.Unchecked => CheckState.Indeterminate,
        CheckState.Indeterminate => CheckState.Checked,
        _ => CheckState.Unchecked
    };

    private static CheckBox Legend(CheckState state, string text, int y) => new()
    {
        Text = text,
        ThreeState = true,
        AutoCheck = false,
        CheckState = state,
        Location = new Point(10, y),
        Size = new Size(125, 23)
    };

    private void RefreshClients()
    {
        var selectedPid = (clients.SelectedItem as ClientItem)?.ProcessId;
        var found = new List<ClientItem>();
        foreach (var name in ClientNames)
        {
            foreach (var process in Process.GetProcessesByName(name))
            {
                using (process)
                {
                    if (process.MainWindowHandle != IntPtr.Zero)
                        found.Add(new ClientItem(process.Id, process.ProcessName, process.MainWindowTitle));
                }
            }
        }

        found = found.DistinctBy(item => item.ProcessId).OrderBy(item => item.ProcessId).ToList();
        if (SameClients(found))
            return;
        clients.BeginUpdate();
        clients.Items.Clear();
        clients.Items.AddRange(found.Cast<object>().ToArray());
        clients.EndUpdate();
        clients.SelectedItem = clients.Items.Cast<ClientItem>().FirstOrDefault(item => item.ProcessId == selectedPid);
        if (clients.SelectedIndex < 0 && clients.Items.Count == 1)
            clients.SelectedIndex = 0;
        targetStatus.Text = found.Count == 0 ? "Nenhum cliente VibeRO aberto foi encontrado." : $"{found.Count} cliente(s) encontrado(s).";
    }

    private bool SameClients(IReadOnlyList<ClientItem> found) =>
        found.Count == clients.Items.Count && found.Select(item => item.ProcessId).SequenceEqual(clients.Items.Cast<ClientItem>().Select(item => item.ProcessId));

    private void SetEnabled(bool value)
    {
        if (value && clients.SelectedItem is not ClientItem)
        {
            MessageBox.Show("Abra o VibeRO e selecione o cliente antes de ligar.", "VibeTools", MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
        }
        if (value && !keyOptions.Values.Any(option => option.CheckState != CheckState.Unchecked))
        {
            MessageBox.Show("Marque ao menos uma tecla antes de ligar.", "VibeTools", MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
        }
        enabled = value;
        ReleaseAllKeys();
        nextRepeat.Clear();
        clientBox.Enabled = !enabled;
        spamBox.Enabled = !enabled;
        delay.Enabled = !enabled;
        if (enabled)
            clientTimer.Stop();
        else
            clientTimer.Start();
        toggle.Text = enabled ? "DESLIGAR" : "LIGAR";
        toggle.BackColor = enabled ? Color.FromArgb(220, 38, 38) : Color.FromArgb(22, 163, 74);
        state.Text = enabled ? "Ligado" : "Desligado";
        state.ForeColor = enabled ? Color.FromArgb(22, 163, 74) : Color.FromArgb(220, 38, 38);
    }

    private void OnPhysicalKey(int virtualKey, bool isDown)
    {
        if (!keyOptions.TryGetValue(virtualKey, out var option) || option.CheckState == CheckState.Unchecked)
            return;
        if (!isDown)
        {
            nextRepeat.Remove(virtualKey);
            pendingClicks.Remove(virtualKey);
            if (pendingRelease.Remove(virtualKey))
                InputSender.SendKeyUp(virtualKey);
            return;
        }
        if (enabled && IsSelectedClientForeground())
            nextRepeat.TryAdd(virtualKey, Environment.TickCount64 + (int)delay.Value);
    }

    private void RepeatIfDue()
    {
        var now = Environment.TickCount64;
        if (mouseReleaseDue != 0 && now >= mouseReleaseDue)
        {
            InputSender.LeftMouseUp();
            mouseReleaseDue = 0;
        }
        foreach (var key in pendingRelease.Where(entry => now >= entry.Value).Select(entry => entry.Key).ToArray())
        {
            InputSender.SendKeyUp(key);
            pendingRelease.Remove(key);
        }
        foreach (var key in pendingClicks.Where(entry => now >= entry.Value).Select(entry => entry.Key).ToArray())
        {
            pendingClicks.Remove(key);
            if (nextRepeat.ContainsKey(key) && IsSelectedClientForeground())
            {
                InputSender.LeftMouseDown();
                mouseReleaseDue = now + 20;
            }
        }

        if (!enabled || !IsSelectedClientForeground())
        {
            ReleaseAllKeys();
            return;
        }
        foreach (var key in nextRepeat.Keys.ToArray())
        {
            if (now < nextRepeat[key] || pendingRelease.ContainsKey(key))
                continue;
            if (!keyOptions.TryGetValue(key, out var option) || option.CheckState == CheckState.Unchecked)
            {
                nextRepeat.Remove(key);
                continue;
            }
            InputSender.SendKeyDown(key);
            pendingRelease[key] = now + 20;
            if (option.CheckState == CheckState.Checked)
                pendingClicks[key] = now + 30;
            nextRepeat[key] = now + (int)delay.Value;
        }
    }

    private bool IsSelectedClientForeground()
    {
        if (clients.SelectedItem is not ClientItem selected)
            return false;
        var window = Native.GetForegroundWindow();
        Native.GetWindowThreadProcessId(window, out var processId);
        return processId == selected.ProcessId;
    }

    private void ReleaseAllKeys()
    {
        foreach (var key in pendingRelease.Keys.ToArray())
            InputSender.SendKeyUp(key);
        pendingRelease.Clear();
        pendingClicks.Clear();
        if (mouseReleaseDue != 0)
            InputSender.LeftMouseUp();
        mouseReleaseDue = 0;
        nextRepeat.Clear();
    }

    protected override void OnFormClosing(FormClosingEventArgs eventArgs)
    {
        if (enabled)
        {
            eventArgs.Cancel = true;
            MessageBox.Show("Clique em DESLIGAR antes de fechar o VibeTools.", "VibeTools", MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
        }
        base.OnFormClosing(eventArgs);
    }

    protected override void Dispose(bool disposing)
    {
        if (disposing)
        {
            spamTimer.Dispose();
            clientTimer.Dispose();
            ReleaseAllKeys();
            keyboardHook.Dispose();
        }
        base.Dispose(disposing);
    }

    private sealed record ClientItem(int ProcessId, string ProcessName, string WindowTitle)
    {
        public override string ToString() => $"{ProcessName}.exe  |  PID {ProcessId}  |  {WindowTitle}";
    }
}

internal sealed class KeyboardHook : IDisposable
{
    private const int WhKeyboardLl = 13;
    private const uint Injected = 0x10;
    private readonly Action<int, bool> callback;
    private readonly Native.HookProc hookProc;
    private readonly IntPtr handle;

    internal KeyboardHook(Action<int, bool> callback)
    {
        this.callback = callback;
        hookProc = Handle;
        handle = Native.SetWindowsHookEx(WhKeyboardLl, hookProc, IntPtr.Zero, 0);
        if (handle == IntPtr.Zero)
            throw new InvalidOperationException("Nao foi possivel iniciar o monitor de teclado.");
    }

    private IntPtr Handle(int code, IntPtr message, IntPtr data)
    {
        if (code >= 0)
        {
            var key = Marshal.PtrToStructure<Native.KeyboardData>(data);
            if ((key.Flags & Injected) == 0)
            {
                var msg = message.ToInt32();
                if (msg is 0x0100 or 0x0104) callback((int)key.VirtualKey, true);
                if (msg is 0x0101 or 0x0105) callback((int)key.VirtualKey, false);
            }
        }
        return Native.CallNextHookEx(handle, code, message, data);
    }

    public void Dispose() => Native.UnhookWindowsHookEx(handle);
}

internal static class InputSender
{
    internal static void SendKeyDown(int key) => SendKey(key, keyUp: false);
    internal static void SendKeyUp(int key) => SendKey(key, keyUp: true);

    private static void SendKey(int key, bool keyUp)
    {
        var scanCode = (ushort)Native.MapVirtualKey((uint)key, 0);
        Send([Native.Input.Keyboard(scanCode, 0x0008 | (keyUp ? 0x0002u : 0u))]);
    }

    internal static void LeftMouseDown() => Send([Native.Input.Mouse(0x0002)]);
    internal static void LeftMouseUp() => Send([Native.Input.Mouse(0x0004)]);

    private static void Send(Native.Input[] inputs) => Native.SendInput((uint)inputs.Length, inputs, Marshal.SizeOf<Native.Input>());
}

internal static class Native
{
    internal delegate IntPtr HookProc(int code, IntPtr message, IntPtr data);
    [StructLayout(LayoutKind.Sequential)] internal struct KeyboardData { internal uint VirtualKey, ScanCode, Flags, Time; internal UIntPtr ExtraInfo; }
    [StructLayout(LayoutKind.Sequential)]
    internal struct Input
    {
        internal uint Type;
        internal InputUnion Data;
        internal static Input Keyboard(ushort scanCode, uint flags) => new() { Type = 1, Data = new() { Keyboard = new() { ScanCode = scanCode, Flags = flags } } };
        internal static Input Mouse(uint flags) => new() { Type = 0, Data = new() { Mouse = new() { Flags = flags } } };
    }
    [StructLayout(LayoutKind.Explicit)] internal struct InputUnion { [FieldOffset(0)] internal MouseInput Mouse; [FieldOffset(0)] internal KeyboardInput Keyboard; [FieldOffset(0)] internal HardwareInput Hardware; }
    [StructLayout(LayoutKind.Sequential)] internal struct MouseInput { internal int X, Y; internal uint MouseData, Flags, Time; internal UIntPtr ExtraInfo; }
    [StructLayout(LayoutKind.Sequential)] internal struct KeyboardInput { internal ushort VirtualKey, ScanCode; internal uint Flags, Time; internal UIntPtr ExtraInfo; }
    [StructLayout(LayoutKind.Sequential)] internal struct HardwareInput { internal uint Message; internal ushort ParamL, ParamH; }
    [DllImport("user32.dll", SetLastError = true)] internal static extern IntPtr SetWindowsHookEx(int id, HookProc callback, IntPtr module, uint threadId);
    [DllImport("user32.dll")] [return: MarshalAs(UnmanagedType.Bool)] internal static extern bool UnhookWindowsHookEx(IntPtr hook);
    [DllImport("user32.dll")] internal static extern IntPtr CallNextHookEx(IntPtr hook, int code, IntPtr message, IntPtr data);
    [DllImport("user32.dll")] internal static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] internal static extern uint GetWindowThreadProcessId(IntPtr window, out uint processId);
    [DllImport("user32.dll")] internal static extern uint MapVirtualKey(uint code, uint mapType);
    [DllImport("user32.dll", SetLastError = true)] internal static extern uint SendInput(uint count, Input[] inputs, int size);
}
