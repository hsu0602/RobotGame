#!/usr/bin/env bash
set -euo pipefail

# 指定輸出執行檔名稱，方便 judge.py 呼叫
g++ -std=c++17 -O2 -pipe -static -s bots/A.cpp -o bots/A
g++ -std=c++17 -O2 -pipe -static -s bots/B.cpp -o bots/B
echo "[Compile] Done."
