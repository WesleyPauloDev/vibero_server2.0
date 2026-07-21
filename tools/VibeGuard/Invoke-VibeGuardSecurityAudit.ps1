[CmdletBinding()]
param(
    [string]$ServerRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path,

    [string]$MySqlExe = 'C:\xampp\mysql\bin\mysql.exe',

    [string]$OutputPath = (Join-Path (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path 'log\vibeguard-security.jsonl'),

    [ValidateRange(1, 1440)]
    [int]$StackWindowMinutes = 10,

    [ValidateRange(100, 30000000)]
    [int]$StackPositiveThreshold = 10000,

    [ValidateRange(1, 168)]
    [int]$ItemTraceWindowHours = 24,

    [ValidateRange(1, 1440)]
    [int]$LoginFailureWindowMinutes = 15,

    [ValidateRange(1, 10000)]
    [int]$LoginFailureThreshold = 25,

    [int[]]$PrivilegedGroupIds = @(99)
)

$ErrorActionPreference = 'Stop'

function Read-ConfigValue {
    param(
        [string[]]$Lines,
        [string]$Key
    )

    $match = @($Lines | Where-Object { $_ -match ('^\s*' + [regex]::Escape($Key) + '\s*:') }) | Select-Object -Last 1
    if (-not $match) {
        throw "Configuracao ausente: $Key"
    }
    return (($match -split ':', 2)[1]).Trim()
}

function Add-Finding {
    param(
        [System.Collections.Generic.List[object]]$Findings,
        [ValidateSet('info', 'low', 'medium', 'high', 'critical')]
        [string]$Severity,
        [string]$Category,
        [string]$Code,
        [string]$Message,
        [hashtable]$Details = @{}
    )

    $Findings.Add([pscustomobject]@{
        observed_at = (Get-Date).ToUniversalTime().ToString('o')
        severity = $Severity
        category = $Category
        code = $Code
        message = $Message
        details = $Details
    })
}

function Invoke-MySqlRows {
    param(
        [string]$HostName,
        [int]$Port,
        [string]$User,
        [string]$Password,
        [string]$Database,
        [string]$Query
    )

    $previousPassword = $env:MYSQL_PWD
    try {
        $env:MYSQL_PWD = $Password
        $output = @(& $MySqlExe "--host=$HostName" "--port=$Port" "--user=$User" "--database=$Database" --batch --raw --skip-column-names -e $Query 2>&1)
        if ($LASTEXITCODE -ne 0) {
            throw "A consulta de seguranca falhou sem expor credenciais: $($output -join ' ')"
        }
        return @($output)
    }
    finally {
        if ($null -eq $previousPassword) {
            Remove-Item Env:MYSQL_PWD -ErrorAction SilentlyContinue
        }
        else {
            $env:MYSQL_PWD = $previousPassword
        }
    }
}

$root = (Resolve-Path -LiteralPath $ServerRoot).Path
$interPath = Join-Path $root 'conf\inter_athena.conf'
$loginPath = Join-Path $root 'conf\login_athena.conf'
$charPath = Join-Path $root 'conf\char_athena.conf'
$logConfigPath = Join-Path $root 'conf\log_athena.conf'

if (-not (Test-Path -LiteralPath $MySqlExe)) {
    throw "Cliente MySQL nao encontrado: $MySqlExe"
}

$inter = Get-Content -LiteralPath $interPath
$login = Get-Content -LiteralPath $loginPath
$char = Get-Content -LiteralPath $charPath
$logConfig = Get-Content -LiteralPath $logConfigPath
$findings = [System.Collections.Generic.List[object]]::new()

$databaseUser = Read-ConfigValue $inter 'map_server_id'
$databasePassword = Read-ConfigValue $inter 'map_server_pw'
if ($databaseUser -ieq 'root') {
    Add-Finding $findings critical authentication database_admin_account `
        'Os servidores usam uma conta administrativa do banco. Crie usuarios de privilegio minimo.'
}
if ($databasePassword.Length -lt 16) {
    Add-Finding $findings high authentication weak_database_password `
        'A senha interna do banco e curta. O valor nunca foi registrado pelo auditor.' `
        @{ minimum_recommended_length = 16 }
}

$md5Passwords = Read-ConfigValue $login 'use_MD5_passwords'
if ($md5Passwords -match '^(yes|on|1)$') {
    Add-Finding $findings high authentication legacy_md5_passwords `
        'As senhas das contas usam MD5, inadequado contra vazamento offline do banco.'
}

$ipBanEnabled = Read-ConfigValue $login 'ipban_enable'
$dynamicFailureBan = Read-ConfigValue $login 'ipban_dynamic_pass_failure_ban'
$failureLimit = [int](Read-ConfigValue $login 'ipban_dynamic_pass_failure_ban_limit')
$failureDuration = [int](Read-ConfigValue $login 'ipban_dynamic_pass_failure_ban_duration')
if ($ipBanEnabled -notmatch '^(yes|on|1)$' -or $dynamicFailureBan -notmatch '^(yes|on|1)$') {
    Add-Finding $findings high authentication login_bruteforce_protection_disabled `
        'O bloqueio dinamico contra tentativas repetidas de login esta desativado.'
}
elseif ($failureLimit -gt 10 -or $failureDuration -lt 5) {
    Add-Finding $findings medium authentication login_bruteforce_policy_weak `
        'O bloqueio de tentativas de login esta ativo, mas os limites merecem revisao.' `
        @{ failure_limit = $failureLimit; ban_duration_minutes = $failureDuration }
}
else {
    Add-Finding $findings info authentication login_bruteforce_protection_enabled `
        'O bloqueio dinamico contra tentativas repetidas de login esta ativo.' `
        @{ failure_limit = $failureLimit; ban_duration_minutes = $failureDuration }
}

$pinEnabled = Read-ConfigValue $char 'pincode_enabled'
if ($pinEnabled -notmatch '^(yes|on|1)$') {
    Add-Finding $findings medium authentication secondary_pin_disabled `
        'O PIN secundario do servidor de personagens esta desativado.'
}
else {
    $pinForce = Read-ConfigValue $char 'pincode_force'
    $pinMaxTry = [int](Read-ConfigValue $char 'pincode_maxtry')
    if ($pinForce -notmatch '^(yes|on|1)$' -or $pinMaxTry -gt 3) {
        Add-Finding $findings medium authentication secondary_pin_policy_weak `
            'O PIN esta ativo, mas nao e obrigatorio ou aceita tentativas demais.' `
            @{ forced = $pinForce; maximum_attempts = $pinMaxTry }
    }
    else {
        Add-Finding $findings info authentication secondary_pin_enabled `
            'O PIN secundario obrigatorio esta ativo.' `
            @{ maximum_attempts = $pinMaxTry }
    }
}

$clientHashCheck = Read-ConfigValue $login 'client_hash_check'
if ($clientHashCheck -notmatch '^(yes|on|1)$') {
    Add-Finding $findings medium client_integrity server_client_hash_check_disabled `
        'O login-server nao exige seu proprio hash de cliente; o manifesto do VibeGuard continua sendo a verificacao principal.'
}

$mainHost = Read-ConfigValue $inter 'map_server_ip'
$mainPort = [int](Read-ConfigValue $inter 'map_server_port')
$mainDatabase = Read-ConfigValue $inter 'map_server_db'

$privilegedGroups = @($PrivilegedGroupIds | Sort-Object -Unique)
if ($privilegedGroups.Count -gt 0) {
    $groupList = ($privilegedGroups | ForEach-Object { [int]$_ }) -join ','
    $privilegedRows = @(Invoke-MySqlRows $mainHost $mainPort $databaseUser $databasePassword $mainDatabase `
        "SELECT group_id, COUNT(*) FROM login WHERE group_id IN ($groupList) GROUP BY group_id ORDER BY group_id;")
    $totalPrivileged = 0
    $countsByGroup = @{}
    foreach ($row in $privilegedRows) {
        $columns = $row -split "`t", 2
        $countsByGroup[$columns[0]] = [int]$columns[1]
        $totalPrivileged += [int]$columns[1]
    }
    Add-Finding $findings $(if ($totalPrivileged -gt 3) { 'medium' } else { 'info' }) authentication privileged_account_inventory `
        'Inventario de contas privilegiadas concluido sem registrar nomes ou senhas.' `
        @{ configured_group_ids = $privilegedGroups; total_accounts = $totalPrivileged; counts_by_group = $countsByGroup }
}

$duplicateQuery = @'
SELECT unique_id, COUNT(*) AS copies,
       GROUP_CONCAT(CONCAT(location, ':', owner_id, ':', row_id) ORDER BY location SEPARATOR ',') AS locations
FROM (
    SELECT 'inventory' AS location, char_id AS owner_id, id AS row_id, unique_id FROM inventory WHERE unique_id <> 0
    UNION ALL SELECT 'cart', char_id, id, unique_id FROM cart_inventory WHERE unique_id <> 0
    UNION ALL SELECT 'storage', account_id, id, unique_id FROM storage WHERE unique_id <> 0
    UNION ALL SELECT 'guild_storage', guild_id, id, unique_id FROM guild_storage WHERE unique_id <> 0
    UNION ALL SELECT 'mail', id, `index`, unique_id FROM mail_attachments WHERE unique_id <> 0
    UNION ALL SELECT 'auction', seller_id, auction_id, unique_id FROM auction WHERE unique_id <> 0
) AS item_locations
GROUP BY unique_id
HAVING COUNT(*) > 1
ORDER BY copies DESC, unique_id
LIMIT 200;
'@

$duplicates = @(Invoke-MySqlRows $mainHost $mainPort $databaseUser $databasePassword $mainDatabase $duplicateQuery)
if ($duplicates.Count -eq 0) {
    Add-Finding $findings info item_integrity no_duplicate_unique_ids `
        'Nenhum unique_id duplicado foi encontrado nas tabelas persistentes.'
}
else {
    foreach ($row in $duplicates) {
        $columns = $row -split "`t", 3
        Add-Finding $findings critical item_integrity duplicate_unique_id `
            'O mesmo unique_id existe simultaneamente em mais de um local.' `
            @{ unique_id = $columns[0]; copies = [int]$columns[1]; locations = $columns[2] }
    }
}

$engineQuery = @'
SELECT ENGINE, COUNT(*)
FROM information_schema.TABLES
WHERE TABLE_SCHEMA = DATABASE()
  AND TABLE_NAME IN ('inventory','cart_inventory','storage','guild_storage','mail_attachments','auction')
GROUP BY ENGINE;
'@
$engines = @(Invoke-MySqlRows $mainHost $mainPort $databaseUser $databasePassword $mainDatabase $engineQuery)
foreach ($row in $engines) {
    $columns = $row -split "`t", 2
    if ($columns[0] -ine 'InnoDB') {
        Add-Finding $findings high item_integrity non_transactional_item_tables `
            'Tabelas persistentes de itens usam um mecanismo sem transacoes robustas.' `
            @{ engine = $columns[0]; table_count = [int]$columns[1] }
    }
}

$invalidAmountQuery = @'
SELECT location, owner_id, row_id, nameid, amount
FROM (
    SELECT 'inventory' AS location, char_id AS owner_id, id AS row_id, nameid, amount FROM inventory
    UNION ALL SELECT 'cart', char_id, id, nameid, amount FROM cart_inventory
    UNION ALL SELECT 'storage', account_id, id, nameid, amount FROM storage
    UNION ALL SELECT 'guild_storage', guild_id, id, nameid, amount FROM guild_storage
    UNION ALL SELECT 'mail', id, `index`, nameid, amount FROM mail_attachments
) AS persistent_stacks
WHERE amount <= 0 OR amount > 30000
ORDER BY amount DESC
LIMIT 200;
'@
$invalidAmounts = @(Invoke-MySqlRows $mainHost $mainPort $databaseUser $databasePassword $mainDatabase $invalidAmountQuery)
if ($invalidAmounts.Count -eq 0) {
    Add-Finding $findings info item_integrity stack_amounts_valid `
        'Nenhuma pilha persistente excede o limite estrutural de 30000 itens.'
}
else {
    foreach ($row in $invalidAmounts) {
        $columns = $row -split "`t", 5
        Add-Finding $findings critical item_integrity invalid_stack_amount `
            'Uma pilha persistente possui quantidade estruturalmente invalida.' `
            @{ location = $columns[0]; owner_id = $columns[1]; row_id = $columns[2]; nameid = $columns[3]; amount = $columns[4] }
    }
}

$logHost = Read-ConfigValue $inter 'log_db_ip'
$logPort = [int](Read-ConfigValue $inter 'log_db_port')
$logUser = Read-ConfigValue $inter 'log_db_id'
$logPassword = Read-ConfigValue $inter 'log_db_pw'
$logDatabase = Read-ConfigValue $inter 'log_db_db'
$logRows = @(Invoke-MySqlRows $logHost $logPort $logUser $logPassword $logDatabase `
    'SELECT COUNT(*), COALESCE(DATE_FORMAT(MAX(time), ''%Y-%m-%dT%H:%i:%s''), '''') FROM picklog;')

if ($logRows.Count -eq 1) {
    $columns = $logRows[0] -split "`t", 2
    Add-Finding $findings info item_logging picklog_status `
        'A trilha SQL de movimentacao de itens foi consultada.' `
        @{ event_count = [long]$columns[0]; latest_event = $columns[1] }
}

$volumeQuery = @"
SELECT char_id, nameid,
       SUM(CASE WHEN amount > 0 THEN amount ELSE 0 END) AS positive_amount,
       SUM(amount) AS net_amount,
       COUNT(*) AS event_count
FROM picklog
WHERE time >= NOW() - INTERVAL $StackWindowMinutes MINUTE
GROUP BY char_id, nameid
HAVING positive_amount >= $StackPositiveThreshold
ORDER BY positive_amount DESC
LIMIT 200;
"@
$volumeRows = @(Invoke-MySqlRows $logHost $logPort $logUser $logPassword $logDatabase $volumeQuery)
foreach ($row in $volumeRows) {
    $columns = $row -split "`t", 5
    Add-Finding $findings medium item_integrity unusual_stackable_volume `
        'Volume positivo elevado de um item em janela curta; requer revisao do fluxo no picklog.' `
        @{
            char_id = $columns[0]
            nameid = $columns[1]
            positive_amount = $columns[2]
            net_amount = $columns[3]
            event_count = $columns[4]
            window_minutes = $StackWindowMinutes
        }
}

$uniqueFlowQuery = @"
SELECT unique_id, nameid,
       SUM(amount) AS net_amount,
       COUNT(DISTINCT char_id) AS character_count,
       COUNT(*) AS event_count
FROM picklog
WHERE unique_id <> 0
  AND time >= NOW() - INTERVAL $ItemTraceWindowHours HOUR
GROUP BY unique_id, nameid
HAVING net_amount > 1
ORDER BY net_amount DESC
LIMIT 200;
"@
$uniqueFlowRows = @(Invoke-MySqlRows $logHost $logPort $logUser $logPassword $logDatabase $uniqueFlowQuery)
if ($uniqueFlowRows.Count -eq 0) {
    Add-Finding $findings info item_integrity unique_item_flow_balanced `
        'Nenhum identificador unico apresentou criacao liquida de multiplas copias na janela auditada.' `
        @{ window_hours = $ItemTraceWindowHours }
}
else {
    foreach ($row in $uniqueFlowRows) {
        $columns = $row -split "`t", 5
        Add-Finding $findings high item_integrity unique_item_positive_balance `
            'Um identificador unico apresentou saldo positivo superior a uma copia; revise toda a trilha antes de agir.' `
            @{
                unique_id = $columns[0]
                nameid = $columns[1]
                net_amount = $columns[2]
                character_count = $columns[3]
                event_count = $columns[4]
                window_hours = $ItemTraceWindowHours
            }
    }
}

$failedLoginRows = @(Invoke-MySqlRows $logHost $logPort $logUser $logPassword $logDatabase @"
SELECT COUNT(*) AS failure_count,
       COUNT(DISTINCT ip) AS source_count,
       COUNT(DISTINCT user) AS account_name_count
FROM loginlog
WHERE rcode IN (0, 1)
  AND time >= NOW() - INTERVAL $LoginFailureWindowMinutes MINUTE;
"@)
if ($failedLoginRows.Count -eq 1) {
    $columns = $failedLoginRows[0] -split "`t", 3
    $failureCount = [int]$columns[0]
    Add-Finding $findings $(if ($failureCount -ge $LoginFailureThreshold) { 'medium' } else { 'info' }) authentication recent_login_failures `
        'Tentativas de login recusadas foram resumidas sem registrar IP, usuario ou senha.' `
        @{
            failure_count = $failureCount
            distinct_sources = [int]$columns[1]
            distinct_account_names = [int]$columns[2]
            window_minutes = $LoginFailureWindowMinutes
            review_threshold = $LoginFailureThreshold
        }
}

$zenyLogging = Read-ConfigValue $logConfig 'log_zeny'
if ([int]$zenyLogging -eq 0) {
    Add-Finding $findings medium item_logging zeny_logging_disabled `
        'Alteracoes de Zeny nao estao sendo registradas; duplicacao de moeda teria pouca rastreabilidade.'
}

$outputDirectory = Split-Path -Parent $OutputPath
New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null
foreach ($finding in $findings) {
    Add-Content -LiteralPath $OutputPath -Value ($finding | ConvertTo-Json -Compress -Depth 5) -Encoding utf8
}

$findings | Sort-Object @{ Expression = {
    switch ($_.severity) { critical { 0 } high { 1 } medium { 2 } low { 3 } default { 4 } }
} }, category | Format-Table severity, category, code, message -AutoSize

Write-Host "Relatorio local: $OutputPath"
Write-Host 'Nenhuma senha, lista de processos ou dado de hardware foi registrado ou enviado.'
