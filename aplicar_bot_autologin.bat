@echo off
setlocal
cd /d "%~dp0"

tasklist /FI "IMAGENAME eq map-server.exe" 2>NUL | find /I "map-server.exe" >NUL
if not errorlevel 1 (
    echo Feche o map-server antes de aplicar esta atualizacao.
    pause
    exit /b 1
)

if not exist "map-server-bot-autologin.exe" (
    echo Arquivo map-server-bot-autologin.exe nao encontrado.
    pause
    exit /b 1
)

if not exist "backup_codex\map-server-before-bot-autologin-20260831.exe" (
    copy /Y "map-server.exe" "backup_codex\map-server-before-bot-autologin-20260831.exe" >NUL
)

copy /Y "map-server-bot-autologin.exe" "map-server.exe" >NUL
if errorlevel 1 (
    echo Nao foi possivel atualizar o map-server.exe.
    pause
    exit /b 1
)

echo Atualizacao aplicada. Agora inicie o map-server normalmente.
pause
