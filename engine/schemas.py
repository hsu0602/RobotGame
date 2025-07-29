"""
使用 Pydantic 2.x 來定義 log 檔結構。
方便：
  • 在 engine/judge.py 產生 log 前即做型別檢查
  • GUI 讀取時先用 MatchLog.validate() 確保格式正確
"""

from __future__ import annotations
from typing import List, Literal
from pydantic import BaseModel, Field, ConfigDict

Move   = Literal["UP", "DOWN", "LEFT", "RIGHT"]
Item   = Literal["b", "m", "n", "s", "t"]
Cell   = Literal[".", "A", "B", "X", *list("bmnst")]

# -------------------------------------------------------------------- #
class PlayerSnapshot(BaseModel):
    model_config = ConfigDict(extra="forbid")

    x: int = Field(..., ge=0)
    y: int = Field(..., ge=0)
    score: int
    frozen: int

class GridRow(BaseModel):
    """一行字串：'A . . b t .'  → 只有做輕量驗證"""
    __root__: str

    @property
    def cells(self) -> List[Cell]:
        parts: List[str] = self.__root__.split()
        for c in parts:
            if c not in {".", "A", "B", "X", *list("bmnst")}:
                raise ValueError(f"invalid cell '{c}' in grid row")
        return parts

class FrameSnapshot(BaseModel):
    A: PlayerSnapshot
    B: PlayerSnapshot
    grid: List[GridRow]

class SpawnedItem(BaseModel):
    x: int
    y: int
    item: Item

class Frame(BaseModel):
    turn: int                       # 1-based
    before: FrameSnapshot
    new_items: List[SpawnedItem]
    after: FrameSnapshot

class MatchLog(BaseModel):
    """整場比賽的 list[Frame]"""
    __root__: List[Frame]

    @classmethod
    def loads(cls, json_str: str) -> "MatchLog":
        """外部 helper：str → MatchLog（含驗證）"""
        return cls.model_validate_json(json_str)
