/* ====== 基本設定 ====== */
const cvs = document.getElementById("board");
const ctx = cvs.getContext("2d");
const wrap = document.getElementById("boardWrap");

const pickLog = document.getElementById("pickLog");
const toggleBtn = document.getElementById("toggleBtn");
const restartBtn = document.getElementById("restartBtn");
const speedRange = document.getElementById("speedRange");
const speedLabel = document.getElementById("speedLabel");
const turnLabel = document.getElementById("turnLabel");

const scoreAEl = document.getElementById("scoreA");
const scoreBEl = document.getElementById("scoreB");
const scoreAcard = document.getElementById("scoreAcard");
const scoreBcard = document.getElementById("scoreBcard");

/* ====== 內部狀態 ====== */
let replay = [];
let turn = 0;
let playing = false;
let timer = null;

let rows = 0, cols = 0;
let baseDelay = 300;          // ms
let speedMul = 1.0;           // 倍速
let DPR = window.devicePixelRatio || 1; // HiDPI 支援  :contentReference[oaicite:6]{index=6}

/* ====== 顏色與尺寸 ====== */
const COLORS = {
  grid: getCSS("--grid"),
  gridOdd: getCSS("--grid-odd"),
  mine: getCSS("--mine"),
  star: getCSS("--star"),   // s：黃色星
  good: getCSS("--good"),   // m：綠
  bad: getCSS("--bad"),     // n：紅
  half: getCSS("--half"),   // t：紫
  A: getCSS("--accent-a"),
  B: getCSS("--accent-b"),
  text: getCSS("--text"),
};

function getCSS(varName) {
  return getComputedStyle(document.documentElement).getPropertyValue(varName).trim();
}

/* ====== 事件綁定 ====== */
pickLog.addEventListener("change", async (e) => {
  const f = e.target.files[0];
  if (!f) return;
  replay = JSON.parse(await f.text());
  turn = 0;

  // 解析棋盤尺寸（以第一幀 after.grid）
  const g0 = replay[0]?.after?.grid;
  if (!g0) return;
  rows = g0.length;
  cols = g0[0].split(" ").length;

  // 讓容器維持正確長寬比
  wrap.style.aspectRatio = `${cols} / ${rows}`;

  ensureCanvasSize();
  drawFrame(replay[0].after);
  updateUI(replay[0].after, 1, replay.length);

  toggleBtn.disabled = false;
  restartBtn.disabled = false;
});

toggleBtn.addEventListener("click", () => {
  if (!replay.length) return;
  playing ? pause() : play();
});

restartBtn.addEventListener("click", () => {
  if (!replay.length) return;
  pause();
  turn = 0;
  drawFrame(replay[0].after);
  updateUI(replay[0].after, 1, replay.length);
});

speedRange.addEventListener("input", () => {
  speedMul = parseFloat(speedRange.value || "1");
  speedLabel.textContent = `×${speedMul.toFixed(2).replace(/\.00$/, ".0")}`;
});

window.addEventListener("resize", () => {
  const old = { w: cvs.width, h: cvs.height };
  ensureCanvasSize();
  if (replay.length) drawFrame(replay[Math.min(turn, replay.length - 1)].after);
});

/* ====== 播放控制 ====== */
function play() {
  playing = true;
  toggleBtn.textContent = "⏸ 暫停";
  scheduleNext();
}
function pause() {
  playing = false;
  toggleBtn.textContent = "▶︎ 播放";
  if (timer) {
    clearTimeout(timer);
    timer = null;
  }
}
function scheduleNext() {
  if (!playing) return;
  if (turn >= replay.length) {
    pause();
    return;
  }
  const frame = replay[turn].after;
  drawFrame(frame);
  updateUI(frame, turn + 1, replay.length);
  turn++;

  const delay = baseDelay / speedMul;
  timer = setTimeout(scheduleNext, delay);
}

/* ====== 畫面繪製 ====== */
function ensureCanvasSize() {
  // 取得容器 CSS 尺寸
  const rect = wrap.getBoundingClientRect();
  const cssW = Math.max(50, Math.floor(rect.width));
  const cssH = Math.max(50, Math.floor(rect.height));

  // 設定 canvas 真實像素大小（乘上 DPR），再以 transform 縮放回 CSS 尺寸。
  // 這是 HiDPI 畫布清晰的標準作法。:contentReference[oaicite:7]{index=7}
  DPR = window.devicePixelRatio || 1; // 重新取值以應對使用者拖曳視窗時 DPR 變化
  cvs.width = Math.floor(cssW * DPR);
  cvs.height = Math.floor(cssH * DPR);
  cvs.style.width = cssW + "px";
  cvs.style.height = cssH + "px";

  // 將座標系以 DPR 比例縮放，之後用 CSS 像素思維作圖
  ctx.setTransform(DPR, 0, 0, DPR, 0, 0);
}

function drawFrame(frame) {
  const grid = frame.grid.map((row) => row.split(" "));

  const W = cvs.clientWidth;
  const H = cvs.clientHeight;

  ctx.clearRect(0, 0, W, H);

  const cell = Math.floor(Math.min(W / cols, H / rows));
  const offsetX = Math.floor((W - cell * cols) / 2);
  const offsetY = Math.floor((H - cell * rows) / 2);

  // 背景（棋盤）
  for (let i = 0; i < rows; i++) {
    for (let j = 0; j < cols; j++) {
      const x = offsetX + j * cell;
      const y = offsetY + i * cell;

      // 交錯底色以提升可讀性
      ctx.fillStyle = (i + j) % 2 === 0 ? COLORS.gridOdd : "#ffffff00";
      ctx.fillRect(x, y, cell, cell);

      // 輕格線
      ctx.strokeStyle = COLORS.grid;
      ctx.lineWidth = 1;
      ctx.strokeRect(x + 0.5, y + 0.5, cell - 1, cell - 1);
    }
  }

  // 畫道具 / 玩家
  for (let i = 0; i < rows; i++) {
    for (let j = 0; j < cols; j++) {
      const c = grid[i][j];
      const x = offsetX + j * cell;
      const y = offsetY + i * cell;

      switch (c) {
        case "b":
        case "m":
        case "n":
        case "s":
        case "t":
          drawItem(c, x, y, cell);
          break;
        case "A":
        case "B":
          drawPlayer(c, x, y, cell);
          break;
        case "X":
          drawOverlap(x, y, cell);
          break;
        default:
          break;
      }
    }
  }

  // 角落座標與目前 Turn（小字）
  ctx.fillStyle = COLORS.text;
  ctx.font = "12px ui-sans-serif, system-ui, -apple-system";
  ctx.textAlign = "right";
  ctx.textBaseline = "bottom";
  ctx.fillText(`Turn ${turn}`, W - 8, H - 6);
}

/* ====== 圖示繪製 ====== */
/* 玩家：彩色圓片 + 字母 */
function drawPlayer(who, x, y, s) {
  const r = Math.floor(s * 0.36);
  const cx = x + s / 2;
  const cy = y + s / 2;

  ctx.beginPath();
  ctx.fillStyle = who === "A" ? COLORS.A : COLORS.B;
  ctx.arc(cx, cy, r, 0, Math.PI * 2);
  ctx.fill();

  ctx.fillStyle = "#fff";
  ctx.font = `${Math.floor(s * 0.38)}px ui-sans-serif, system-ui`;
  ctx.textAlign = "center";
  ctx.textBaseline = "middle";
  ctx.fillText(who, cx, cy + (who === "A" ? 1 : 0));
}

/* 同格交戰 X：咖啡色十字與框 */
function drawOverlap(x, y, s) {
  ctx.save();
  ctx.translate(x, y);
  ctx.strokeStyle = "#795548";
  ctx.lineWidth = Math.max(2, Math.floor(s * 0.06));
  ctx.beginPath();
  ctx.moveTo(s * 0.2, s * 0.2);
  ctx.lineTo(s * 0.8, s * 0.8);
  ctx.moveTo(s * 0.8, s * 0.2);
  ctx.lineTo(s * 0.2, s * 0.8);
  ctx.stroke();
  ctx.restore();
}

function drawPlusMinus(cx, cy, s, bgColor, sign /* +1 or -1 */) {
  const r = s * 0.22;
  // 圓底
  ctx.beginPath();
  ctx.fillStyle = bgColor;
  ctx.arc(cx, cy, r, 0, Math.PI * 2);
  ctx.fill();

  // 白色線條（+ 或 -）
  ctx.strokeStyle = "#fff";
  ctx.lineWidth = Math.max(2, Math.floor(s * 0.06));
  ctx.lineCap = "round";

  // 橫線
  ctx.beginPath();
  ctx.moveTo(cx - r * 0.6, cy);
  ctx.lineTo(cx + r * 0.6, cy);
  ctx.stroke();

  // 直線（若為 +）
  if (sign > 0) {
    ctx.beginPath();
    ctx.moveTo(cx, cy - r * 0.6);
    ctx.lineTo(cx, cy + r * 0.6);
    ctx.stroke();
  }
}


function drawItem(type, x, y, s) {
  const cx = x + s / 2;
  const cy = y + s / 2;

  switch (type) {
    case "b": // 地雷：維持原樣
      ctx.fillStyle = COLORS.mine;
      ctx.beginPath();
      ctx.arc(cx, cy + s * 0.06, s * 0.26, 0, Math.PI * 2);
      ctx.fill();
      ctx.strokeStyle = COLORS.mine;
      ctx.lineWidth = Math.max(1, s * 0.04);
      ctx.beginPath();
      ctx.moveTo(cx + s * 0.12, cy - s * 0.06);
      ctx.quadraticCurveTo(cx + s * 0.26, cy - s * 0.22, cx + s * 0.12, cy - s * 0.28);
      ctx.stroke();
      break;

    case "s": // 倍增：黃色星星（維持）
      drawStar(cx, cy, 5, s * 0.22, s * 0.09, COLORS.star, "#c28e0e");
      break;

    case "t": // 折半：改用與 s 相同星形，但塗成紫色
      drawStar(cx, cy, 5, s * 0.22, s * 0.09, COLORS.half, "#6b21a8");
      break;

    case "m": // 加分：綠色圓底 + 白色加號
      drawPlusMinus(cx, cy, s, COLORS.good, +1);
      break;

    case "n": // 減分：紅色圓底 + 白色減號
      drawPlusMinus(cx, cy, s, COLORS.bad, -1);
      break;
  }
}


/* 五角星工具函式（畫 s） */
function drawStar(cx, cy, spikes, outerR, innerR, fill, stroke) {
  let rot = Math.PI / 2 * 3;
  let x = cx;
  let y = cy;
  const step = Math.PI / spikes;

  ctx.beginPath();
  ctx.moveTo(cx, cy - outerR);
  for (let i = 0; i < spikes; i++) {
    x = cx + Math.cos(rot) * outerR;
    y = cy + Math.sin(rot) * outerR;
    ctx.lineTo(x, y);
    rot += step;

    x = cx + Math.cos(rot) * innerR;
    y = cy + Math.sin(rot) * innerR;
    ctx.lineTo(x, y);
    rot += step;
  }
  ctx.lineTo(cx, cy - outerR);
  ctx.closePath();
  ctx.fillStyle = fill;
  ctx.fill();
  if (stroke) {
    ctx.strokeStyle = stroke;
    ctx.lineWidth = 1;
    ctx.stroke();
  }
}

/* ====== UI 更新 ====== */
function updateUI(frame, turnNow, turnTotal) {
  turnLabel.textContent = `Turn ${turnNow} / ${turnTotal}`;
  scoreAEl.textContent = frame.A.score;
  scoreBEl.textContent = frame.B.score;

  // 領先者高亮
  scoreAcard.classList.remove("leader");
  scoreBcard.classList.remove("leader");
  if (frame.A.score > frame.B.score) {
    scoreAcard.classList.add("leader");
  } else if (frame.B.score > frame.A.score) {
    scoreBcard.classList.add("leader");
  }
}
