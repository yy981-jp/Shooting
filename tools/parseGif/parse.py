import sys
import json
from pathlib import Path
from PIL import Image

if len(sys.argv) < 2:
    print("usage: parse <gif>")
    sys.exit(1)

gif_path = Path(sys.argv[1])

# 出力フォルダ (拡張子除く)
out_dir = Path.cwd() / gif_path.stem
out_dir.mkdir(exist_ok=True)

im = Image.open(gif_path)

width, height = im.size
frames = im.n_frames

# duration(ms) → sec
duration = im.info.get("duration", 0) / 1000

# フレーム保存
for i in range(frames):
    im.seek(i)
    frame = im.convert("RGBA")
    frame.save(out_dir / f"{i}.png")

# meta.json
meta = {
    "version": 1,
    "size": [width, height],
    "duration": duration,
    "frames": frames
}

with open(out_dir / "meta.json", "w", encoding="utf-8") as f:
    json.dump(meta, f, indent=4)
