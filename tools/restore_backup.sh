#!/usr/bin/env bash
set -euo pipefail

if [[ $# -lt 1 ]]; then
	echo "Usage: $0 <backup_dir> [target_dir]" >&2
	exit 1
fi

BACKUP_DIR="$(cd "$1" && pwd)"
TARGET_DIR="${2:-$(pwd)}"
ROOT_DIR="$(cd "$TARGET_DIR" && pwd)"
TMP_DIR="$(mktemp -d)"

cleanup() {
	rm -rf "$TMP_DIR"
}
trap cleanup EXIT

read_conf() {
	local key="$1"
	local file="$2"
	awk -F': *' -v key="$key" '
		$0 ~ "^[[:space:]]*" key ":" {
			value = substr($0, index($0, ":") + 1)
			sub(/^[[:space:]]+/, "", value)
			sub(/[[:space:]]+$/, "", value)
			print value
			exit
		}
	' "$file"
}

write_mysql_defaults() {
	local defaults_file="$1"
	local host="$2"
	local port="$3"
	local user="$4"
	local password="$5"

	cat >"$defaults_file" <<EOF
[client]
host=$host
port=$port
user=$user
password=$password
EOF
	chmod 600 "$defaults_file"
}

restore_database() {
	local dump_file="$1"
	local host="$2"
	local port="$3"
	local user="$4"
	local password="$5"
	local database="$6"
	local defaults_file="$TMP_DIR/${database}.cnf"

	write_mysql_defaults "$defaults_file" "$host" "$port" "$user" "$password"
	echo "Restoring database [$database] from $(basename "$dump_file")"
	gunzip -c "$dump_file" | mysql --defaults-extra-file="$defaults_file" "$database"
}

FILES_ARCHIVE="$BACKUP_DIR/files.tar.gz"
if [[ ! -f "$FILES_ARCHIVE" ]]; then
	echo "Backup archive not found: $FILES_ARCHIVE" >&2
	exit 1
fi

mkdir -p "$ROOT_DIR"
echo "Extracting files into $ROOT_DIR"
tar -xzf "$FILES_ARCHIVE" -C "$ROOT_DIR"

INTER_CONF="$ROOT_DIR/conf/inter_athena.conf"
if [[ ! -f "$INTER_CONF" ]]; then
	echo "inter_athena.conf not found after extraction: $INTER_CONF" >&2
	exit 1
fi

LOGIN_HOST="$(read_conf login_server_ip "$INTER_CONF")"
LOGIN_PORT="$(read_conf login_server_port "$INTER_CONF")"
LOGIN_USER="$(read_conf login_server_id "$INTER_CONF")"
LOGIN_PASS="$(read_conf login_server_pw "$INTER_CONF")"
LOGIN_DB="$(read_conf login_server_db "$INTER_CONF")"

WEB_HOST="$(read_conf web_server_ip "$INTER_CONF")"
WEB_PORT="$(read_conf web_server_port "$INTER_CONF")"
WEB_USER="$(read_conf web_server_id "$INTER_CONF")"
WEB_PASS="$(read_conf web_server_pw "$INTER_CONF")"
WEB_DB="$(read_conf web_server_db "$INTER_CONF")"

LOG_HOST="$(read_conf log_db_ip "$INTER_CONF")"
LOG_PORT="$(read_conf log_db_port "$INTER_CONF")"
LOG_USER="$(read_conf log_db_id "$INTER_CONF")"
LOG_PASS="$(read_conf log_db_pw "$INTER_CONF")"
LOG_DB="$(read_conf log_db_db "$INTER_CONF")"

GAME_DUMP="$(find "$BACKUP_DIR/databases" -maxdepth 1 -type f -name 'game-*.sql.gz' | head -n 1)"
WEB_DUMP="$(find "$BACKUP_DIR/databases" -maxdepth 1 -type f -name 'web-*.sql.gz' | head -n 1)"
LOG_DUMP="$(find "$BACKUP_DIR/databases" -maxdepth 1 -type f -name 'log-*.sql.gz' | head -n 1)"

if [[ -n "$GAME_DUMP" ]]; then
	restore_database "$GAME_DUMP" "$LOGIN_HOST" "$LOGIN_PORT" "$LOGIN_USER" "$LOGIN_PASS" "$LOGIN_DB"
fi

if [[ -n "$WEB_DUMP" && -n "$WEB_DB" ]]; then
	restore_database "$WEB_DUMP" "$WEB_HOST" "$WEB_PORT" "$WEB_USER" "$WEB_PASS" "$WEB_DB"
fi

if [[ -n "$LOG_DUMP" && -n "$LOG_DB" ]]; then
	restore_database "$LOG_DUMP" "$LOG_HOST" "$LOG_PORT" "$LOG_USER" "$LOG_PASS" "$LOG_DB"
fi

echo "Restore completed into: $ROOT_DIR"
