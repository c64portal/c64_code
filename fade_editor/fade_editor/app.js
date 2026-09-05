(function () {
  "use strict";

  const COLS = 40;
  const ROWS = 25;
  const CELL = 20;
  const WIDTH = COLS * CELL;
  const HEIGHT = ROWS * CELL;
  const MIN_PHASE = 1;
  const MAX_PHASE = 126;
  const HISTORY_LIMIT = 80;

  const canvas = document.getElementById("editorCanvas");
  const ctx = canvas.getContext("2d");
  const phaseValue = document.getElementById("phaseValue");
  const phaseInput = document.getElementById("phaseInput");
  const statusEl = document.getElementById("status");
  const fadeInput = document.getElementById("fadeInput");
  const imageInput = document.getElementById("imageInput");
  const showValuesInput = document.getElementById("showValuesInput");
  const showGridInput = document.getElementById("showGridInput");
  const opacityInput = document.getElementById("opacityInput");
  const undoBtn = document.getElementById("undoBtn");
  const redoBtn = document.getElementById("redoBtn");
  const manualBtn = document.getElementById("manualBtn");
  const manualView = document.getElementById("manualView");
  const workbench = document.querySelector(".workbench");

  const data = new Uint8Array(COLS * ROWS);
  const undoStack = [];
  const redoStack = [];

  let phase = 1;
  let image = null;
  let imageUrl = "";
  let fadeFileName = "women.fade";
  let isDrawing = false;
  let drawValue = 0;
  let activePointerId = null;
  let dragSnapshot = null;
  let lastCell = -1;

  function indexOf(x, y) {
    return x + y * COLS;
  }

  function clampPhase(value) {
    const parsed = Number.parseInt(value, 10);
    if (Number.isNaN(parsed)) {
      return phase;
    }
    return Math.max(MIN_PHASE, Math.min(MAX_PHASE, parsed));
  }

  function setPhase(next) {
    phase = clampPhase(next);
    phaseValue.value = String(phase);
    phaseValue.textContent = String(phase);
    phaseInput.value = String(phase);
    render();
  }

  function setStatus(text) {
    statusEl.textContent = text;
  }

  function snapshot() {
    return new Uint8Array(data);
  }

  function pushUndo(before) {
    undoStack.push(before);
    if (undoStack.length > HISTORY_LIMIT) {
      undoStack.shift();
    }
    redoStack.length = 0;
    updateHistoryButtons();
  }

  function updateHistoryButtons() {
    undoBtn.disabled = undoStack.length === 0;
    redoBtn.disabled = redoStack.length === 0;
  }

  function restore(bytes) {
    data.set(bytes);
    render();
  }

  function clearFade() {
    if (!data.some(Boolean)) {
      setStatus("Already clear.");
      return;
    }
    const before = snapshot();
    data.fill(0);
    pushUndo(before);
    setStatus("Fade map cleared.");
    render();
  }

  function phaseColor(value) {
    const hue = (value * 19) % 360;
    return `hsl(${hue} 78% 42%)`;
  }

  function drawBackground() {
    ctx.clearRect(0, 0, WIDTH, HEIGHT);
    ctx.fillStyle = "#cfd4db";
    ctx.fillRect(0, 0, WIDTH, HEIGHT);

    if (image) {
      ctx.save();
      ctx.globalAlpha = Number(opacityInput.value) / 100;
      ctx.drawImage(image, 0, 0, WIDTH, HEIGHT);
      ctx.restore();
    }
  }

  function drawCells() {
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.font = "11px ui-monospace, SFMono-Regular, Menlo, Consolas, monospace";

    for (let y = 0; y < ROWS; y += 1) {
      for (let x = 0; x < COLS; x += 1) {
        const idx = indexOf(x, y);
        const value = data[idx];
        if (value === 0) {
          continue;
        }

        const px = x * CELL;
        const py = y * CELL;
        ctx.fillStyle = phaseColor(value);
        ctx.globalAlpha = 0.88;
        ctx.fillRect(px, py, CELL, CELL);
        ctx.globalAlpha = 1;

        if (showValuesInput.checked) {
          ctx.fillStyle = "#ffffff";
          ctx.fillText(String(value), px + CELL / 2, py + CELL / 2 + 0.5);
        }
      }
    }
  }

  function drawGrid() {
    if (!showGridInput.checked) {
      return;
    }

    ctx.beginPath();
    ctx.strokeStyle = "rgba(17, 24, 39, 0.38)";
    ctx.lineWidth = 1;

    for (let x = 0; x <= COLS; x += 1) {
      const px = x * CELL + 0.5;
      ctx.moveTo(px, 0);
      ctx.lineTo(px, HEIGHT);
    }

    for (let y = 0; y <= ROWS; y += 1) {
      const py = y * CELL + 0.5;
      ctx.moveTo(0, py);
      ctx.lineTo(WIDTH, py);
    }

    ctx.stroke();
  }

  function render() {
    drawBackground();
    drawCells();
    drawGrid();
  }

  function cellFromEvent(event) {
    const rect = canvas.getBoundingClientRect();
    const scaleX = WIDTH / rect.width;
    const scaleY = HEIGHT / rect.height;
    const x = Math.floor((event.clientX - rect.left) * scaleX / CELL);
    const y = Math.floor((event.clientY - rect.top) * scaleY / CELL);

    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) {
      return null;
    }

    return { x, y, idx: indexOf(x, y) };
  }

  function paintCell(event) {
    const cell = cellFromEvent(event);
    if (!cell || cell.idx === lastCell) {
      return;
    }

    lastCell = cell.idx;
    data[cell.idx] = drawValue;
    render();
  }

  function beginDraw(event) {
    if (event.button === 1) {
      return;
    }

    event.preventDefault();
    canvas.setPointerCapture(event.pointerId);
    activePointerId = event.pointerId;
    dragSnapshot = snapshot();
    drawValue = event.button === 2 ? 0 : phase;
    isDrawing = true;
    lastCell = -1;
    paintCell(event);
  }

  function moveDraw(event) {
    if (!isDrawing || event.pointerId !== activePointerId) {
      return;
    }

    event.preventDefault();
    paintCell(event);
  }

  function endDraw(event) {
    if (!isDrawing || event.pointerId !== activePointerId) {
      return;
    }

    canvas.releasePointerCapture(event.pointerId);
    isDrawing = false;
    activePointerId = null;
    lastCell = -1;

    if (dragSnapshot && !arraysEqual(dragSnapshot, data)) {
      pushUndo(dragSnapshot);
      setStatus(drawValue === 0 ? "Cells erased." : `Painted phase ${drawValue}.`);
    }

    dragSnapshot = null;
  }

  function arraysEqual(a, b) {
    if (a.length !== b.length) {
      return false;
    }

    for (let i = 0; i < a.length; i += 1) {
      if (a[i] !== b[i]) {
        return false;
      }
    }

    return true;
  }

  function loadFadeFile(file) {
    const reader = new FileReader();
    reader.onload = function () {
      const bytes = new Uint8Array(reader.result);
      if (bytes.length < data.length) {
        setStatus("Fade file is shorter than 1000 bytes.");
        return;
      }

      const before = snapshot();
      data.set(bytes.slice(0, data.length));
      fadeFileName = file.name || fadeFileName;
      pushUndo(before);
      setStatus(`Loaded ${file.name}.`);
      render();
    };
    reader.readAsArrayBuffer(file);
  }

  function loadImageFile(file) {
    const nextImage = new Image();
    const nextUrl = URL.createObjectURL(file);
    nextImage.onload = function () {
      if (imageUrl) {
        URL.revokeObjectURL(imageUrl);
      }
      imageUrl = nextUrl;
      image = nextImage;
      setStatus(`Loaded picture ${file.name}.`);
      render();
    };
    nextImage.onerror = function () {
      URL.revokeObjectURL(nextUrl);
      setStatus("Could not load picture.");
    };
    nextImage.src = nextUrl;
  }

  function saveFade() {
    const blob = new Blob([data], { type: "application/octet-stream" });
    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");
    link.href = url;
    link.download = fadeFileName;
    document.body.appendChild(link);
    link.click();
    link.remove();
    URL.revokeObjectURL(url);
    setStatus("Saved 1000-byte fade file.");
  }

  function setManualVisible(isVisible) {
    manualView.hidden = !isVisible;
    workbench.hidden = isVisible;
    manualBtn.setAttribute("aria-pressed", String(isVisible));
    setStatus(isVisible ? "Manual open." : "Ready. Left paint, right erase.");
  }

  document.getElementById("loadFadeBtn").addEventListener("click", () => fadeInput.click());
  document.getElementById("loadImageBtn").addEventListener("click", () => imageInput.click());
  document.getElementById("saveFadeBtn").addEventListener("click", saveFade);
  document.getElementById("clearBtn").addEventListener("click", clearFade);
  manualBtn.addEventListener("click", () => setManualVisible(manualView.hidden));

  document.getElementById("prevBtn").addEventListener("click", () => setPhase(phase - 1));
  document.getElementById("nextBtn").addEventListener("click", () => setPhase(phase + 1));
  document.getElementById("prev10Btn").addEventListener("click", () => setPhase(phase - 10));
  document.getElementById("next10Btn").addEventListener("click", () => setPhase(phase + 10));

  undoBtn.addEventListener("click", function () {
    if (undoStack.length === 0) {
      return;
    }
    redoStack.push(snapshot());
    restore(undoStack.pop());
    updateHistoryButtons();
    setStatus("Undo.");
  });

  redoBtn.addEventListener("click", function () {
    if (redoStack.length === 0) {
      return;
    }
    undoStack.push(snapshot());
    restore(redoStack.pop());
    updateHistoryButtons();
    setStatus("Redo.");
  });

  phaseInput.addEventListener("change", () => setPhase(phaseInput.value));
  showValuesInput.addEventListener("change", render);
  showGridInput.addEventListener("change", render);
  opacityInput.addEventListener("input", render);

  fadeInput.addEventListener("change", function () {
    const file = fadeInput.files && fadeInput.files[0];
    if (file) {
      loadFadeFile(file);
    }
    fadeInput.value = "";
  });

  imageInput.addEventListener("change", function () {
    const file = imageInput.files && imageInput.files[0];
    if (file) {
      loadImageFile(file);
    }
    imageInput.value = "";
  });

  canvas.addEventListener("pointerdown", beginDraw);
  canvas.addEventListener("pointermove", moveDraw);
  canvas.addEventListener("pointerup", endDraw);
  canvas.addEventListener("pointercancel", endDraw);
  canvas.addEventListener("contextmenu", (event) => event.preventDefault());

  window.addEventListener("keydown", function (event) {
    if (event.target === phaseInput) {
      return;
    }

    if (event.key === "ArrowLeft") {
      event.preventDefault();
      setPhase(phase - (event.shiftKey ? 10 : 1));
    } else if (event.key === "ArrowRight") {
      event.preventDefault();
      setPhase(phase + (event.shiftKey ? 10 : 1));
    } else if ((event.metaKey || event.ctrlKey) && event.key.toLowerCase() === "z") {
      undoBtn.click();
    } else if ((event.metaKey || event.ctrlKey) && event.key.toLowerCase() === "y") {
      redoBtn.click();
    }
  });

  updateHistoryButtons();
  setPhase(1);
})();
