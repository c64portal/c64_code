#!/usr/bin/env python3
from pathlib import Path
from PIL import Image

# Klatka: 96x42 => 4x2 sprite'y => 8 sprite'ów 24x21
SRC_W, SRC_H = 96, 42
GRID_W, GRID_H = 4, 2

SPR_W, SPR_H = 24, 21
FRAMES = 25  # 0000..0024

BASE_DIR = Path(__file__).parent

def sprite24x21_to_c64_64bytes(im_rgba: Image.Image) -> bytes:
    if im_rgba.size != (SPR_W, SPR_H):
        raise ValueError(f"Sprite ma zły rozmiar: {im_rgba.size}, oczekiwane {(SPR_W, SPR_H)}")

    px = im_rgba.load()
    out = bytearray()

    for y in range(SPR_H):
        b0 = b1 = b2 = 0
        for x in range(SPR_W):
            r, g, b, a = px[x, y]
            # sztywno: czarny = tło (0), każdy inny kolor = 1
            bit = 0 if (r == 0 and g == 0 and b == 0) else 1

            if x < 8:
                b0 = (b0 << 1) | bit
            elif x < 16:
                b1 = (b1 << 1) | bit
            else:
                b2 = (b2 << 1) | bit

        out.extend((b0, b1, b2))

    out.append(0)  # padding do 64 bajtów
    return bytes(out)

def convert_one_frame(frame: int):
    png_path = BASE_DIR / f"{frame:04d}.png"
    if not png_path.exists():
        raise FileNotFoundError(f"Brak pliku: {png_path.name}")

    im = Image.open(png_path).convert("RGBA")
    if im.size != (SRC_W, SRC_H):
        raise ValueError(f"{png_path.name}: zły rozmiar {im.size}, oczekiwane {(SRC_W, SRC_H)}")

    frame_bytes = bytearray()

    # kolejność: wierszami (top->bottom), lewo->prawo:
    # 0 1 2 3
    # 4 5 6 7
    for gy in range(GRID_H):
        for gx in range(GRID_W):
            x0 = gx * SPR_W
            y0 = gy * SPR_H
            tile = im.crop((x0, y0, x0 + SPR_W, y0 + SPR_H))
            frame_bytes += sprite24x21_to_c64_64bytes(tile)

    out_path = BASE_DIR / f"{frame:04d}.bin"
    out_path.write_bytes(frame_bytes)

    if len(frame_bytes) != 8 * 64:
        raise RuntimeError(f"{out_path.name}: zły rozmiar wyjścia {len(frame_bytes)} (oczekiwane 512)")

def main():
    for frame in range(FRAMES):
        convert_one_frame(frame)

    print("OK: zapisano 0000.bin .. 0024.bin (po 512 bajtów każdy).")

if __name__ == "__main__":
    main()