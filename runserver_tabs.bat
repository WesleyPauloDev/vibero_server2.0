@echo off
REM Caminho da pasta onde estão os arquivos .bat
set SERVER_DIR=C:\Users\wesley\Desktop\Conf_MeuRag

REM Abre tudo no Windows Terminal com 4 abas
wt -w 0 ^
  new-tab -d "%SERVER_DIR%" cmd /k "logserv.bat" ^
  ; new-tab -d "%SERVER_DIR%" cmd /k "charserv.bat" ^
  ; new-tab -d "%SERVER_DIR%" cmd /k "webserv.bat" ^
  ; new-tab -d "%SERVER_DIR%" cmd /k "mapserv.bat"
