from __future__ import annotations
import random
from dataclasses import dataclass, asdict
from typing import List, Literal, Tuple

# --- 基本型別 ------------------------------------------------------

Move = Literal["UP", "DOWN", "LEFT", "RIGHT"]
Item = Literal["b", "m", "n", "s", "t"]   # 對應 md 規則
Cell = str                                # '.', 'A', 'B', 'X', or item letter

@dataclass
class PlayerState:
    x: int
    y: int
    score: int
    frozen: int = 0    # 受地雷影響停幾回合

# --- 遊戲主體 ------------------------------------------------------

class TreasureGame:
    MAX_TURN = 1000
    TIME_LIMIT_MS = 1000

    def __init__(self, m: int = 8, n: int = 7, seed: int | None = None) -> None:
        self.rng = random.Random(seed)
        self.M, self.N = m, n
        self.turn = 1
        # 地圖 '.' = 空地
        self.grid: List[List[Cell]] = [["." for _ in range(n)] for _ in range(m)]
        # 起點
        self.A = PlayerState(0, 0, 0)
        self.B = PlayerState(m - 1, n - 1, 0)
        self._place_players()

    # ------------------- 公開 API -------------------

    def step(self, a_move: Move, b_move: Move) -> dict:
        """執行一個回合，依序處理 A → B，並回傳可序列化 dict（供 log）"""
        frame = {
            "turn": self.turn,
            "before": self._frame_snapshot(),
            "new_items": self._spawn_items()
        }

        self._move_player(self.A, a_move)
        self._move_player(self.B, b_move, after_A=True)

        self._battle_if_same_cell()
        frame["after"] = self._frame_snapshot()

        self.turn += 1
        return frame

    def finished(self) -> bool:
        return self.turn > self.MAX_TURN

    # ------------------- 私有工具 -------------------

    def _place_players(self):
        self.grid[self.A.x][self.A.y] = "A"
        self.grid[self.B.x][self.B.y] = "B"

    def _spawn_items(self) -> List[dict]:
        # 每回合隨機挑 0~2 個空格丟道具，可自行調整機率
        spawned = []
        for _ in range(self.rng.randint(0, 2)):
            x, y = self.rng.randrange(self.M), self.rng.randrange(self.N)
            if self.grid[x][y] == ".":
                item = self.rng.choice(list("bmnst"))
                self.grid[x][y] = item
                spawned.append({"x": x, "y": y, "item": item})
        return spawned

    def _move_player(self, p: PlayerState, move: Move, after_A: bool = False):
        if p.frozen > 0:
            p.frozen -= 1
            return

        dx, dy = {"UP": (-1, 0), "DOWN": (1, 0),
                  "LEFT": (0, -1), "RIGHT": (0, 1)}[move]

        nx, ny = p.x + dx, p.y + dy
        if not (0 <= nx < self.M and 0 <= ny < self.N):
            # 走出界 → 視為原地
            return

        # 清掉舊位置標記
        self.grid[p.x][p.y] = "."
        p.x, p.y = nx, ny

        cell = self.grid[nx][ny]
        if cell in "bmnst":
            self._consume_item(p, cell)
        # 暫時不標記棋子，先等另一位玩家走完；收尾再標 X / A / B
        self.grid[nx][ny] = "."

    def _consume_item(self, p: PlayerState, item: Item):
        if item == "b":
            p.frozen = 3
        elif item == "m":
            p.score += 1
        elif item == "n":
            p.score -= 1
        elif item == "s":
            p.score *= 2
        elif item == "t":
            p.score //= 2    # toward 0
        # 消失
        # (格子已在 _move_player 中設為 ".")

    def _battle_if_same_cell(self):
        if (self.A.x, self.A.y) != (self.B.x, self.B.y):
            self.grid[self.A.x][self.A.y] = "A"
            self.grid[self.B.x][self.B.y] = "B"
            return

        # battle!
        if self.A.score > self.B.score:
            self.A.score += 2
            self.B.score -= 2
        elif self.B.score > self.A.score:
            self.B.score += 2
            self.A.score -= 2
        # 平手無事

        self.grid[self.A.x][self.A.y] = "X"   # 同格

    def _frame_snapshot(self) -> dict:
        return {
            "A": asdict(self.A),
            "B": asdict(self.B),
            "grid": [" ".join(row) for row in self.grid],
        }
