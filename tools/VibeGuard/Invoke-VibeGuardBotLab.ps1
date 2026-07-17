[CmdletBinding()]
param(
    [ValidateRange(1, 65535)]
    [int]$ProcessId,

    [ValidatePattern('^F([1-9]|1[0-2])$')]
    [string]$SkillKey = 'F1',

    [ValidateRange(50, 120)]
    [int]$Cycles = 60,

    [ValidateRange(300, 2000)]
    [int]$IntervalMs = 600,

    [ValidateRange(60, 300)]
    [int]$MoveOffset = 140
)

$ErrorActionPreference = 'Stop'

$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = [Security.Principal.WindowsPrincipal]::new($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'Execute este script em um PowerShell aberto como administrador.'
}

$guardPath = 'C:\Users\wesley\Desktop\MeuRag_VibeGuard_Test\start.exe'
$guards = @(Get-CimInstance Win32_Process | Where-Object {
    $_.Name -eq 'start.exe' -and $_.ExecutablePath -eq $guardPath
})

if ($guards.Count -eq 0) {
    throw 'Nenhum VibeGuard do cliente de laboratorio esta ativo.'
}

$guardIds = @($guards.ProcessId)
$candidates = @(Get-CimInstance Win32_Process | Where-Object {
    $_.Name -eq 'VibeRO-Client.exe' -and $_.ParentProcessId -in $guardIds
})

if ($ProcessId -gt 0) {
    $candidates = @($candidates | Where-Object ProcessId -eq $ProcessId)
}

if ($candidates.Count -ne 1) {
    $ids = @($candidates.ProcessId) -join ', '
    throw "Era esperado exatamente um VibeRO-Client.exe iniciado pelo VibeGuard. Candidatos: $ids"
}

$target = Get-Process -Id $candidates[0].ProcessId
if ($target.MainWindowHandle -eq 0) {
    throw 'O cliente de laboratorio nao possui uma janela de jogo pronta.'
}

Add-Type @'
using System;
using System.Runtime.InteropServices;

public static class VibeGuardLabInput
{
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left, Top, Right, Bottom; }

    [DllImport("user32.dll")]
    public static extern bool SetForegroundWindow(IntPtr hWnd);

    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);

    [DllImport("user32.dll")]
    public static extern bool SetCursorPos(int x, int y);

    [DllImport("user32.dll")]
    public static extern void mouse_event(uint flags, uint dx, uint dy, uint data, UIntPtr extraInfo);

    [DllImport("user32.dll")]
    public static extern void keybd_event(byte virtualKey, byte scanCode, uint flags, UIntPtr extraInfo);

    public static void Click(int x, int y)
    {
        SetCursorPos(x, y);
        mouse_event(0x0002, 0, 0, 0, UIntPtr.Zero);
        mouse_event(0x0004, 0, 0, 0, UIntPtr.Zero);
    }

    public static void PressFunctionKey(int number)
    {
        byte key = (byte)(0x70 + number - 1);
        keybd_event(key, 0, 0, UIntPtr.Zero);
        keybd_event(key, 0, 0x0002, UIntPtr.Zero);
    }
}
'@

$functionNumber = [int]$SkillKey.Substring(1)
$rect = [VibeGuardLabInput+RECT]::new()
if (-not [VibeGuardLabInput]::GetWindowRect($target.MainWindowHandle, [ref]$rect)) {
    throw 'Nao foi possivel medir a janela do cliente.'
}

$centerX = [int](($rect.Left + $rect.Right) / 2)
$centerY = [int](($rect.Top + $rect.Bottom) / 2)
$leftX = $centerX - $MoveOffset
$rightX = $centerX + $MoveOffset

Write-Host "VibeGuard BotLab: VibeRO-Client.exe PID $($target.Id)"
Write-Host "Ciclos: $Cycles | intervalo: $IntervalMs ms | skill: $SkillKey"
Write-Host 'O mouse sera controlado durante o teste. Pressione Ctrl+C para interromper.'
Write-Host 'Iniciando em 3 segundos...'
Start-Sleep -Seconds 3

[VibeGuardLabInput]::SetForegroundWindow($target.MainWindowHandle) | Out-Null
Start-Sleep -Milliseconds 300

for ($cycle = 1; $cycle -le $Cycles; $cycle++) {
    $x = if (($cycle % 2) -eq 0) { $leftX } else { $rightX }
    [VibeGuardLabInput]::Click($x, $centerY)

    if (($cycle % 2) -eq 0) {
        [VibeGuardLabInput]::PressFunctionKey($functionNumber)
    }

    Write-Progress -Activity 'VibeGuard BotLab' -Status "Ciclo $cycle de $Cycles" -PercentComplete (($cycle / $Cycles) * 100)
    Start-Sleep -Milliseconds $IntervalMs
}

Write-Progress -Activity 'VibeGuard BotLab' -Completed
Write-Host 'Teste finalizado. Consulte @guardstatus e log/vibeguard-suspicion.log.'
