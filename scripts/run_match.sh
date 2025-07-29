#!/usr/bin/env bash
#
# 一鍵：編譯 → 對戰 → 自動開啟 GUI 觀看
#
# 使用方式：
#   bash scripts/run_match.sh            # 亂數種子 = 當前 timestamp
#   bash scripts/run_match.sh 12345      # 指定種子
#
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

SEED=${1:-$(date +%s)}

echo "[1/3] Compile bots ..."
bash "$ROOT/scripts/compile_bots.sh"

echo "[2/3] Run match (seed=$SEED) ..."
python "$ROOT/engine/judge.py" --seed "$SEED"

LATEST_LOG=$(ls -1t "$ROOT/logs/matches" | head -n1)
echo "[3/3] Open GUI with $LATEST_LOG ..."

# macOS：open，Linux (xdg-open)，WSL/Windows 可能改成 start。
if command -v xdg-open &>/dev/null; then
  xdg-open "$ROOT/gui/index.html"
elif command -v open &>/dev/null; then
  open "$ROOT/gui/index.html"
else
  echo ">> 手動在瀏覽器開啟：$ROOT/gui/index.html"
fi
