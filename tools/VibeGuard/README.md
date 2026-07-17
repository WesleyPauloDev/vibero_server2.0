# VibeGuard - laboratorio

Esta pasta inicia um anti-cheat defensivo para o VibeRO. A primeira fase somente
gera um manifesto de integridade dos arquivos explicitamente permitidos em
`guard-policy.json` e valida sua assinatura ECDSA P-256.

## Limites de privacidade da fase 1

- nao captura tela, teclado ou area de transferencia;
- nao enumera documentos, navegadores, janelas ou processos do usuario;
- `verify` e `launch` leem somente manifesto, assinatura e arquivos explicitamente listados;
- nao inclui caminho absoluto, nome do Windows ou identificador de hardware no manifesto;
- `verify` e `generate` nao realizam conexoes de rede;
- `launch` e `session-test` nao usam o proxy do sistema; servidores remotos usam
  HTTPS, exceto quando HTTP e habilitado explicitamente na configuracao assinada;
- nao aplica banimentos.

Os comandos administrativos `keygen` e `sign` acessam apenas os caminhos de
chave informados pelo administrador e nao fazem parte da execucao do jogador.

## Sessao observacional com rAthena

O `web-server` oferece cinco rotas de sessao:

- `/vibeguard/v1/session/open`;
- `/vibeguard/v1/session/claim`;
- `/vibeguard/v1/session/auto-claim`;
- `/vibeguard/v1/session/heartbeat`;
- `/vibeguard/v1/session/close`.

As configuracoes reproduziveis estao em `vibeguard-web.conf.example`. O hash
permitido deve ser atualizado e o manifesto reassinado a cada release aprovada.

A abertura confere o hash de uma versao aprovada e devolve um token aleatorio e
um codigo de pareamento de uso unico. Heartbeats usam `Authorization: Bearer`,
sequencia crescente e expiram sem atividade. Repeticao de sequencia retorna
HTTP 409.

O cliente envia versao do protocolo, versao do VibeGuard, hash do manifesto e
sequencia. Nunca solicita ou envia login ou senha. Para o vinculo automatico, o
web-server compara temporariamente o endereco da conexao com `last_ip`, que o
rAthena ja registra no login, e procura apenas logins ocorridos depois da abertura
da sessao. O IP nao e copiado para a tabela nem para o log do VibeGuard. Hardware
ID, caminhos, captura de tela e processos nao sao enviados. `@guard CODIGO`
permanece apenas como fallback administrativo.

Depois que `start.exe` termina, o guard procura localmente somente um novo
processo com o nome exato `Ragexe`, ignorando instancias que ja estavam abertas.
Nome e PID nao sao enviados nem gravados no log; a consulta serve apenas para
manter a sessao enquanto o cliente local estiver aberto.

## Pareamento e observacao de padroes

Ao abrir `Iniciar-VibeRO-Teste.cmd`, copie a linha exibida pelo launcher e use-a
no personagem que acabou de entrar:

```text
@guard 0123456789ab
```

`@guardstatus` informa se existe uma sessao ativa vinculada e mostra a pontuacao
observacional. Somente sessoes vinculadas e com heartbeat recente sao analisadas.
Contas de GM ficam fora da deteccao automatica.

Na interface grafica atual nao existe campo de conta. Abra `start.exe` e entre
normalmente; o vinculo ocorre depois do login. Para multi-conta, abra uma nova
instancia de `start.exe` depois de concluir o login anterior. Cada sessao ignora
contas ja vinculadas e aguarda o proximo login do mesmo endereco. O comando
`@guard` acima permanece como fallback de laboratorio.

O mapa mede apenas os intervalos entre movimento, ataque, coleta de item e uso de
habilidade. Uma janela de 24 intervalos muito regulares soma suspeita; a pontuacao
cai gradualmente quando o padrao deixa de ocorrer. Aos 30 pontos aparecem um aviso
preventivo no rodape e uma caixa `mes`. Aos 60 pontos, respeitando 30 minutos de
intervalo, os dois avisos sao repetidos com a indicacao de suspeita persistente.
Nenhuma dessas acoes bane, prende,
desconecta ou altera itens do jogador.

Os eventos suspeitos ficam em `log/vibeguard-suspicion.log`, contendo somente
conta, personagem, pontuacao, tipo de evento e horario UTC. O log de sessao fica
em `log/vibeguard-observation.log`; nenhum dos dois grava IP ou hardware ID.

Para testar os avisos com uma conta administradora:

```text
@guardtest 1
@guardtest 2
```

O primeiro comando simula o aviso inicial. O segundo simula a suspeita persistente;
ambos devem exibir a mensagem no rodape e a caixa `mes`. Esses comandos nao
acrescentam punicao e `@guardtest` e restrito ao grupo de administrador.

Para exercitar a deteccao automatica com uma conta comum no cliente local, existe
`Invoke-VibeGuardBotLab.ps1`. Ele aceita somente um `VibeRO-Client.exe` iniciado
pelo VibeGuard, controla o mouse por um numero finito de ciclos e nao le memoria nem
seleciona alvos. Execute em PowerShell elevado, com a skill de teste em uma tecla
F1-F12:

```powershell
Set-Location "C:\Users\wesley\Desktop\Conf_MeuRag2"
.\tools\VibeGuard\Invoke-VibeGuardBotLab.ps1 -SkillKey F1
```

O teste alterna cliques de movimento a cada 600 ms e pressiona a skill a cada dois
ciclos. `Ctrl+C` interrompe imediatamente. Nao use esse gerador fora do servidor
local de laboratorio.

Teste a sessao sem abrir o jogo:

```powershell
.\VibeGuard.exe session-test `
  --server "http://127.0.0.1:8888" `
  --manifest .\vibeguard-manifest.json `
  --signature .\vibeguard-manifest.sig
```

As pastas `ScreenShot`, `Replay`, `savedata` e `memo` foram deixadas vazias na
copia de laboratorio. Elas nao devem entrar em manifestos nem em telemetria.

## Gerar o manifesto

Execute na raiz do repositorio:

```powershell
dotnet run --project tools/VibeGuard/VibeGuard.Manifest -- generate `
  --game-dir "C:\Users\wesley\Desktop\MeuRag_VibeGuard_Test" `
  --policy "tools\VibeGuard\guard-policy.json" `
  --output "tools\VibeGuard\artifacts\client-manifest.json"
```

Assine o manifesto com a chave privada mantida somente pelo administrador:

```powershell
dotnet run --project tools/VibeGuard/VibeGuard.Manifest -- sign `
  --manifest "tools\VibeGuard\artifacts\client-manifest.json" `
  --private-key "tools\VibeGuard\artifacts\keys\vibeguard-lab-private.pem" `
  --signature "tools\VibeGuard\artifacts\client-manifest.sig"
```

Verifique a copia sem iniciar o jogo:

```powershell
dotnet run --project tools/VibeGuard/VibeGuard.Manifest -- verify `
  --game-dir "C:\Users\wesley\Desktop\MeuRag_VibeGuard_Test" `
  --manifest "tools\VibeGuard\artifacts\client-manifest.json" `
  --signature "tools\VibeGuard\artifacts\client-manifest.sig"
```

Inicie um executavel protegido somente se a verificacao passar:

```powershell
dotnet run --project tools/VibeGuard/VibeGuard.Manifest -- launch `
  --game-dir "C:\Users\wesley\Desktop\MeuRag_VibeGuard_Test" `
  --manifest "tools\VibeGuard\artifacts\client-manifest.json" `
  --signature "tools\VibeGuard\artifacts\client-manifest.sig" `
  --executable "VibeRO-Client.exe" `
  --server "http://127.0.0.1:8888"
```

O comando `launch` nao injeta DLL nem enumera processos. Depois de verificar os
arquivos aprovados e iniciar o executavel, ele permanece ativo apenas para enviar
o heartbeat da sessao enquanto acompanha localmente o novo processo `Ragexe`.

## Auditoria defensiva do servidor

Execute a auditoria observacional de credenciais, logs e duplicacao persistente:

```powershell
Set-Location "C:\Users\wesley\Desktop\Conf_MeuRag2"
.\tools\VibeGuard\Invoke-VibeGuardSecurityAudit.ps1
```

O auditor procura `unique_id` repetido entre inventario, carrinho, armazens,
correio e leilao; verifica o mecanismo das tabelas e a saude do `picklog`; e
aponta configuracoes fracas sem imprimir senhas. Os achados sao acrescentados em
`log/vibeguard-security.jsonl`. Ele nao bane contas, nao altera itens, nao le
memoria e nao coleta a lista de processos do computador do jogador.

Para itens empilhaveis, ele tambem sinaliza pilhas fora do limite estrutural de
30000 unidades e volumes positivos elevados em uma janela curta. Esses alertas
sao indicios para revisao do `picklog`, nao prova automatica de duplicacao.

## Aplicacao gradual da sessao

As opcoes em `conf/import/battle_conf.txt` controlam a exigencia no map-server:

- `vibeguard_enforcement_mode: 0`: somente observacao;
- `vibeguard_enforcement_mode: 1`: avisa, mas nao desconecta;
- `vibeguard_enforcement_mode: 2`: avisa e desconecta ao fim da tolerancia;
- `vibeguard_warning_seconds`: instante do primeiro aviso;
- `vibeguard_grace_seconds`: prazo total antes da desconexao;
- `vibeguard_exempt_group_level`: nivel minimo isento para recuperacao administrativa.

A desconexao nunca altera estado, tempo de bloqueio ou grupo da conta. O launcher
tambem exige que a sessao autenticada seja aberta antes de iniciar o jogo. Durante
a execucao, ele verifica mudancas nos arquivos assinados e modulos PE novos na
raiz do cliente. A lista de modulos permanece local e nao e enviada ao servidor.

## Teste manual do pacote de laboratorio

### Inicializacao grafica

O pacote nao depende do `.cmd`. `start.exe` e a interface grafica do VibeGuard.
O executavel original e definido no arquivo assinado
`vibeguard-launcher.json`: `VibeRO-Client.exe` no laboratorio e `VibeRO.exe` em
producao. Ao abrir `start.exe`, a janela:

1. valida a assinatura e os hashes, incluindo o proprio launcher;
2. autentica a sessao e vincula a conta automaticamente depois do login;
3. solicita UAC apenas quando inicia o cliente original;
4. permanece na bandeja mantendo o heartbeat enquanto o jogo estiver aberto.

O pacote de rollback da copia de laboratorio fica em
`VibeGuard-Rollback-20260716-GUI`. O `.cmd` legado apenas encaminha para o novo
`start.exe` e nao executa mais a interface de console.

Depois da publicacao, a pasta `MeuRag_VibeGuard_Test` recebe estes arquivos:

- `start.exe`, a interface grafica assinada;
- `VibeRO-Client.exe`, o cliente original;
- `vibeguard-manifest.json`;
- `vibeguard-manifest.sig`;
- `Iniciar-VibeRO-Teste.cmd`, somente como encaminhamento legado.

Abra diretamente `start.exe`. A interface do VibeGuard nao solicita elevacao;
o Windows exibe o UAC para o cliente somente depois que assinatura e integridade
forem aprovadas. Cancelar o UAC impede a abertura do cliente.

O arquivo `vibeguard-launcher.json` define o executavel, a URL da sessao e o
atraso visual. Ele faz parte do manifesto assinado: qualquer alteracao invalida
o pacote. Em producao, prefira sempre uma URL HTTPS. `allowInsecureHttp` existe
somente para uma migracao controlada e deve voltar a `false` depois da
configuracao de TLS.

O manifesto possui assinatura digital e o executavel contem somente a chave
publica confiavel. A chave privada de laboratorio fica em `artifacts/keys`,
diretorio ignorado pelo Git, e nunca deve ser copiada para o cliente.

O manifesto atual cobre executaveis e bibliotecas de seguranca. Os GRFs grandes
ficaram fora desta primeira validacao para manter o ciclo de teste rapido. Uma
fase futura pode adicionar hashes por blocos.

## Estado atual

1. Integridade assinada dos clientes de laboratorio e producao.
2. Sessao autenticada com heartbeat e vinculo automatico no rAthena.
3. Deteccao observacional de regularidade com avisos preventivos e sem banimento.
4. Aplicacao gradual da sessao com aviso e desconexao, sem alterar a conta.

## Publicacao em producao

No pacote de producao, o cliente original e preservado como
`System/VibeRO-Client.exe` e o VibeGuard ocupa o nome `VibeRO.exe`, usando o
icone original do jogo. `start.exe` nao e distribuido em producao. Dessa forma,
tanto o atalho do jogador quanto o `Atualizador.exe`, ao chamarem `VibeRO.exe`,
passam primeiro pela verificacao. A URL fica em
`assets/vibeguard-launcher-production.json` e os dois executaveis fazem parte de
`guard-policy-production.json`. O hash do manifesto precisa ser identico em
`vibeguard-web.conf.production.example` e no cliente distribuido.

Se o atualizador distribuir uma nova versao do proprio `VibeRO.exe`, ele podera
sobrescrever o wrapper. Esse comportamento deve ser validado em uma copia antes
de cada mudanca do atualizador; nesse caso, o pacote precisa reinstalar o wrapper
e reassinar o cliente depois da atualizacao.

Ordem segura de ativacao:

1. compile e inicie primeiro o `web-server` com a configuracao de producao;
2. confirme externamente que a rota de sessao responde;
3. inicie o `map-server` com `vibeguard_enforcement_mode: 1`;
4. valide login, vinculo e heartbeat usando um cliente externo;
5. somente entao altere o modo para `2` e reinicie o `map-server`.

A versao atual aceita HTTP remoto apenas quando `allowInsecureHttp` esta
explicitamente habilitado no arquivo assinado. Isso permite a primeira
implantacao no IP direto, mas HTTPS deve ser a proxima etapa para proteger os
tokens de sessao durante o transporte.
