#!/usr/bin/env bash
set -euo pipefail

# Safe by default: generate to /tmp so local configs are not modified.
# Use --in-place to explicitly write into npc/scripts_custom.conf.
out="/tmp/scripts_custom.ci.conf"

if [[ "${1:-}" == "--in-place" ]]; then
	out="npc/scripts_custom.conf"
fi

: > "$out"
printf "\n" >> "$out"
echo "// Custom Scripts" >> "$out"
find npc/custom \( -name "*.txt" \) | sort | xargs -I % echo "npc: %" >> "$out"

echo "// Test Scripts" >> "$out"
find npc/test \( -name "*.txt" \) | sort | xargs -I % echo "npc: %" >> "$out"

echo "Generated: $out"
