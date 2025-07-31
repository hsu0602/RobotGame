import json, os, subprocess, sys, time
from pathlib import Path
from typing import Optional
from game import TreasureGame
from tqdm import tqdm

ROOT = Path(__file__).resolve().parents[1]
BOT_A = ROOT / "bots" / "A"
BOT_B = ROOT / "bots" / "B"

def run_bot(exe: Path, stdin: str, timeout_ms: int) -> Optional[str]:
    try:
        start = time.perf_counter()
        proc = subprocess.run(
            [str(exe)], input=stdin.encode(),
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            timeout=timeout_ms/1000,
        )
        used = (time.perf_counter() - start) * 1000
        #print(used)
        out = proc.stdout.decode().strip().upper()
        if out not in {"UP", "DOWN", "LEFT", "RIGHT"}:
            raise ValueError(f"invalid output: {out!r}")
        return out
    except subprocess.TimeoutExpired:
        print(f"[Judge] {exe.name} timeout", file=sys.stderr)
        return None

def main(seed: int = 42):
    game = TreasureGame(seed=seed)
    log = []

    bar = tqdm(total = game.MAX_TURN)
        
    while not game.finished():
        frame_input = generate_input(game, perspective="A")
        a_move = run_bot(BOT_A, frame_input, game.TIME_LIMIT_MS) or "UP"
        frame_input = generate_input(game, perspective="B")
        b_move = run_bot(BOT_B, frame_input, game.TIME_LIMIT_MS) or "UP"

        frame = game.step(a_move, b_move)
        log.append(frame)
        
        bar.update(1)

    ts = int(time.time())
    out_file = ROOT / "logs" / "matches" / f"{ts}.json"
    out_file.parent.mkdir(parents=True, exist_ok=True)
    out_file.write_text(json.dumps(log, indent=2), encoding="utf-8")
    print(f"\n[Judge] Saved → {out_file}")

def generate_input(game: TreasureGame, *, perspective: str) -> str:
    """把 Python 內部狀態轉成 Bot.md 規定的 stdin 文字"""
    pA, pB = game.A, game.B
    if perspective == "B":
        pA, pB = pB, pA  # 角色對調
    grid = [row.split() for row in game._frame_snapshot()["grid"]]
    if perspective == "A":
        grid[pA.x][pA.y] = "A"
        grid[pB.x][pB.y] = "B"
    else:
        grid[pA.x][pA.y] = "B"   # 因為他是 B
        grid[pB.x][pB.y] = "A"
    lines = [
        str(game.turn),
        f"{game.M} {game.N}",
        *[" ".join(r) for r in grid],
        str(pA.score),
        str(pB.score),
        perspective,
        ""
    ]
    return "\n".join(lines)

if __name__ == "__main__":
    main()
