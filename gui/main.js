const CELL = 30;
const COLORS = { '.': '#fff', 'b': '#555', 'm': '#4caf50',
                 'n': '#f44336', 's': '#ffeb3b', 't': '#9c27b0',
                 'A': '#2196f3', 'B': '#ff9800', 'X': '#795548' };

let replay = [], turn = 0;
const cvs = document.getElementById('board');
const ctx = cvs.getContext('2d');

document.getElementById('pickLog').addEventListener('change', async e => {
  const f = e.target.files[0];
  replay = JSON.parse(await f.text());
  turn = 0;
  playNext();
});

function playNext() {
  if (turn >= replay.length) return;
  drawFrame(replay[turn].after);
  turn++;
  setTimeout(playNext, 300);   // 0.3 s / frame
}

function drawFrame(frame) {
  const { grid, A, B } = frame;
  cvs.width = grid[0].split(' ').length * CELL;
  cvs.height = grid.length * CELL;
  grid.forEach((row, i) =>
    row.split(' ').forEach((cell, j) => {
      ctx.fillStyle = COLORS[cell];
      ctx.fillRect(j * CELL, i * CELL, CELL, CELL);
      ctx.strokeRect(j * CELL, i * CELL, CELL, CELL);
    })
  );
  // 分數顯示
  ctx.fillStyle = '#000';
  ctx.fillText(`Turn ${turn}   A:${A.score}   B:${B.score}`, 10, 14);
}
