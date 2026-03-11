# Backup do servidor

Este projeto nao esta em Git neste diretorio, entao o caminho mais seguro e:

1. Fazer backup restauravel de arquivos + banco.
2. Versionar no GitHub apenas codigo/configuracoes sem segredos e sem dumps.

## Gerar backup

No diretorio do servidor:

```bash
bash tools/backup_server.sh
```

Isso cria uma pasta em `backups/<timestamp>/` com:

- `files.tar.gz`: configuracoes, scripts, NPCs, DB YAML/SQL e customizacoes.
- `databases/*.sql.gz`: dump do banco principal, web e logs.
- `manifest.txt`: resumo do snapshot.

## Restaurar em outra hospedagem

1. Copie a pasta `backups/<timestamp>/` para o novo servidor.
2. Extraia e restaure:

```bash
bash tools/restore_backup.sh /caminho/para/backups/<timestamp> /caminho/do/novo/rathena
```

3. Se necessario, recompile:

```bash
./configure
make server
```

## O que sincronizar com GitHub

Pode subir:

- `conf/`
- `db/`
- `npc/`
- `src/custom/`
- `sql-files/`
- `tools/`

Nao deve subir:

- senhas reais de banco
- `conf/inter_athena.conf` com credenciais expostas
- dumps de banco
- `log/`
- `build/`
- binarios `*-server`, `*.exe`, `*.pdb`
- `node_modules/`

## Recomendacao pratica

- Rode o backup automaticamente 1x por dia.
- Copie a pasta `backups/` para outro servidor, S3, Google Drive ou armazenamento externo.
- Antes de usar GitHub, separe credenciais em arquivos de import privado ou variaveis de ambiente.
