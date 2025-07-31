import os, shutil, subprocess, tempfile, time, json, pathlib
from fastapi import FastAPI, UploadFile, Form
from fastapi.responses import JSONResponse
from fastapi.middleware.cors import CORSMiddleware

ROOT = pathlib.Path(__file__).resolve().parents[1]
app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.post("/run_match")
async def run_match(
    file_a: UploadFile,
    file_b: UploadFile,
    seed: int = Form(default=int(time.time())),
):
    work = tempfile.mkdtemp(prefix="judge_")
    bots_dir = ROOT / "bots"
    bots_dir.mkdir(exist_ok=True)

    # 儲存上傳檔
    path_a = bots_dir / "A.cpp"
    path_b = bots_dir / "B.cpp"
    path_a.write_bytes(await file_a.read())
    path_b.write_bytes(await file_b.read())

    # 編譯
    try:
        subprocess.check_output(
            ["bash", "scripts/compile_bots.sh"],
            cwd=ROOT, stderr=subprocess.STDOUT, text=True, timeout=30
        )
    except subprocess.CalledProcessError as e:
        return JSONResponse(status_code=400, content={
            "ok": False, "stage": "compile", "msg": e.output[-500:]
        })

    # 對戰
    try:
        judge_out = subprocess.check_output(
            ["python", "engine/judge.py", "--seed", str(seed)],
            cwd=ROOT, stderr=subprocess.STDOUT, text=True, timeout=60
        )
    except subprocess.CalledProcessError as e:
        return JSONResponse(status_code=500, content={
            "ok": False, "stage": "judge", "msg": e.output[-500:]
        })

    # 從 judge console 解析出檔名
    last_line = judge_out.strip().splitlines()[-1]
    # [Judge] Saved → .../logs/matches/1722421356.json
    log_path = last_line.split("→")[-1].strip()

    return {
        "ok": True,
        "log_path": log_path.replace(str(ROOT) + "/", ""), # 簡化路徑
        "stdout": judge_out,
    }