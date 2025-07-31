/* ===========================  基本設定  =========================== */
const CELL      = 64;           // 單格像素
let   speedMul  = 1;            // 回放速度倍率
let   isPaused  = false;

/* 顏色字典（回退用） */
const COLORS = { '.':'#fff', 'bo':'#000000','m':'#4caf50','n':'#f44336',
                 's':'#ffeb3b','t':'#9c27b0','A':'#2196f3','B':'#ff9800','X':'#795548' };

/* 可選 icon – 若 `img/<key>.png` 存在就用圖，否則 fallback 到顏色 */
const imgCache = {};
function loadIcon(key) {
  if (imgCache[key] !== undefined) return imgCache[key];   // 已嘗試載入過
  const img     = new Image();
  img.src       = `img/${key}.png`;
  img.onerror   = () => (imgCache[key] = null);            // 無檔 → null
  img.onload    = () => (imgCache[key] = img);
  imgCache[key] = undefined;                               // loading 中
  return undefined;
}

/* ===========================  DOM  =========================== */
const cvs     = document.getElementById('board');
const ctx     = cvs.getContext('2d');
const pickLog = document.getElementById('pickLog');
const speedEl = document.getElementById('speed');
const speedVal= document.getElementById('speedVal');
const toggleBtn = document.getElementById('toggleBtn');

/* ===========================  資料  =========================== */
let replay = [], turn = 0;

/* ===========================  監聽  =========================== */
pickLog.addEventListener('change', async e => {
  const f = e.target.files[0];
  replay  = JSON.parse(await f.text());
  turn    = 0;
  isPaused = false;
  toggleBtn.textContent = '⏸︎ 暫停';
  playNext();
});

speedEl.addEventListener('input', () => {
  speedMul       = parseFloat(speedEl.value);
  speedVal.textContent = speedMul.toFixed(1) + ' ×';
});

toggleBtn.addEventListener('click', () => {
  isPaused = !isPaused;
  toggleBtn.textContent = isPaused ? '▶️ 繼續' : '⏸︎ 暫停';
  if (!isPaused) playNext();
});

/* ===========================  主迴圈  =========================== */
function playNext() {
  if (isPaused || turn >= replay.length) return;
  drawFrame(replay[turn].after);
  turn++;
  setTimeout(playNext, 300 / speedMul);   // 300 ms 基準
}

/* ===========================  繪圖  =========================== */
function drawFrame(frame) {
  const { grid, A, B } = frame;
  const M = grid.length, N = grid[0].split(' ').length;
  cvs.width  = N * CELL;
  cvs.height = M * CELL;

  grid.forEach((row, i) => row.split(' ').forEach((cell, j) => {
    const px = j * CELL, py = i * CELL;
    if(cell == 'b') cell = 'bo';
    const icon = loadIcon(cell);


    if (icon && icon.complete && icon.naturalWidth) {
      ctx.drawImage(icon, px, py, CELL, CELL);
    } else {
      ctx.fillStyle = COLORS[cell] || '#000';
      ctx.fillRect(px, py, CELL, CELL);
    }
    ctx.strokeStyle = '#999';
    ctx.strokeRect(px, py, CELL, CELL);
  }));

  // scoreboard update
  document.getElementById('turnBox').textContent = `Turn ${turn}`;
  document.getElementById('scoreA').textContent  = `A ${frame.A.score}`;
  document.getElementById('scoreB').textContent  = `B ${frame.B.score}`;
}
