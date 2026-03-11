#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BACKUP_ROOT="${1:-$ROOT_DIR/backups}"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
BACKUP_DIR="$BACKUP_ROOT/$TIMESTAMP"
FILES_ARCHIVE="$BACKUP_DIR/files.tar.gz"
DB_DIR="$BACKUP_DIR/databases"
MANIFEST="$BACKUP_DIR/manifest.txt"
TMP_DIR="$(mktemp -d)"

cleanup() {
	rm -rf "$TMP_DIR"
}
trap cleanup EXIT

require_cmd() {
	local cmd="$1"
	if ! command -v "$cmd" >/dev/null 2>&1; then
		echo "Missing required command: $cmd" >&2
		exit 1
	fi
}

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

dump_database() {
	local label="$1"
	local host="$2"
	local port="$3"
	local user="$4"
	local password="$5"
	local database="$6"
	local defaults_file="$TMP_DIR/${label}.cnf"
	local output_file="$DB_DIR/${label}-${database}.sql.gz"

	write_mysql_defaults "$defaults_file" "$host" "$port" "$user" "$password"
	echo "Dumping database [$label] $database"
	mysqldump \
		--defaults-extra-file="$defaults_file" \
		--single-transaction \
		--quick \
		--no-tablespaces \
		--routines \
		--triggers \
		--default-character-set=utf8mb4 \
		"$database" | gzip -c >"$output_file"
}

require_cmd mysqldump
require_cmd tar
require_cmd gzip

mkdir -p "$DB_DIR"

INTER_CONF="$ROOT_DIR/conf/inter_athena.conf"

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

dump_database "game" "$LOGIN_HOST" "$LOGIN_PORT" "$LOGIN_USER" "$LOGIN_PASS" "$LOGIN_DB"

if [[ -n "$WEB_DB" ]]; then
	dump_database "web" "$WEB_HOST" "$WEB_PORT" "$WEB_USER" "$WEB_PASS" "$WEB_DB"
fi

if [[ -n "$LOG_DB" ]]; then
	dump_database "log" "$LOG_HOST" "$LOG_PORT" "$LOG_USER" "$LOG_PASS" "$LOG_DB"
fi

echo "Creating file archive"
tar -czf "$FILES_ARCHIVE" \
	--exclude='backups' \
	--exclude='build' \
	--exclude='log' \
	--exclude='node_modules' \
	--exclude='run' \
	--exclude='*.pid' \
	-C "$ROOT_DIR" \
	conf \
	db \
	npc \
	src/custom \
	sql-files \
	tools \
	athena-start \
	configure \
	Makefile \
	README.md

cat >"$MANIFEST" <<EOF
timestamp=$TIMESTAMP
root_dir=$ROOT_DIR
files_archive=$(basename "$FILES_ARCHIVE")
database_dumps=$(find "$DB_DIR" -maxdepth 1 -type f -name '*.sql.gz' -printf '%f ' | sed 's/[[:space:]]*$//')
restore_steps=1) extract files.tar.gz 2) import databases 3) rebuild binaries if needed
EOF

echo "Backup created at: $BACKUP_DIR"
