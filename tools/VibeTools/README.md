# VibeTools V1

Aplicativo independente de qualidade de vida para o VibeRO. Nesta primeira
versao, repete somente as habilidades marcadas cujas teclas estiverem fisicamente pressionadas.

- escolhe um cliente aberto pelo PID;
- omite launchers (`start.exe`/`VibeRO.exe`) que nao recebem as teclas do jogo;
- varias teclas simultaneas entre F1-F9, 1-9 e Q-W-E-R-T-Y-U-I-O / A-S-D-F-G-H-J-K-L / Z-X-C-V-B-N-M;
- cada caixa alterna entre desativada, sem clique e com clique esquerdo;
- intervalo configuravel de 50 a 1000 ms;
- bloqueia a configuracao enquanto ligado e para ao soltar a tecla ou tirar o foco do cliente selecionado;
- nao le nem altera a memoria do jogo.
- solicita administrador para poder enviar entradas a clientes Ragnarok elevados.

## Compilar

```powershell
dotnet publish .\tools\VibeTools\VibeTools.csproj -c Release -r win-x64 --self-contained false
```
