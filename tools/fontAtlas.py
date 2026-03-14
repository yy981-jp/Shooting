from PIL import Image, ImageDraw, ImageFont
import json

FONT_PATH = "assets/NotoSansJP.ttf"

ATLAS_W = 1024
ATLAS_H = 512

ASCII_START = 32
ASCII_END = 126
# 95 array


def get_baseline_metrics(font):
    """
    全文字のbbox[1]最小値（上端）とbbox[3]最大値（下端）を返す。
    これによりベースラインを考慮した統一セル高さと描画オフセットが決まる。
    """
    min_top = 0    # bbox[1] の最小値（負になりうる）
    max_bottom = 0 # bbox[3] の最大値
    for c in range(ASCII_START, ASCII_END + 1):
        bbox = font.getbbox(chr(c))
        min_top = min(min_top, bbox[1])
        max_bottom = max(max_bottom, bbox[3])
    # セル高さ = 上端から下端までの全範囲
    cell_h = max_bottom - min_top
    return cell_h, min_top


def exec(draw, x, y, meta, size: str, font):
    if size not in meta:
        meta[size] = {}

    cell_h, min_top = get_baseline_metrics(font)

    for c in range(ASCII_START, ASCII_END + 1):
        ch = chr(c)

        bbox = font.getbbox(ch)
        char_w = bbox[2] - bbox[0]

        if x + char_w >= ATLAS_W:
            x = 0
            y += cell_h + 2

        # 左右余白なし：bbox[0] のオフセットを除去
        # 垂直方向：min_top を基準にベースラインを揃える
        # bbox[1] - min_top により、最も上に出る文字が y=0 になるよう正規化
        draw_x = x - bbox[0]
        # draw_y = y - bbox[1] + (bbox[1] - min_top)  # = y + (bbox[1] - min_top) - bbox[1] = y - min_top
        draw_y = y - min_top

        draw.text((draw_x, draw_y), ch, font=font, fill=(255, 255, 255, 255))

        meta[size][str(ord(ch))] = [x, y, char_w, cell_h]

        x += char_w + 2

    return x, y


font16 = ImageFont.truetype(FONT_PATH, 16)
font32 = ImageFont.truetype(FONT_PATH, 32)
font64 = ImageFont.truetype(FONT_PATH, 64)

atlas = Image.new("RGBA", (ATLAS_W, ATLAS_H), (0, 0, 0, 0))
draw = ImageDraw.Draw(atlas)

x = 0
y = 0

meta = {"version": 1}

x, y = exec(draw, x, y, meta, "16", font16)
x, y = exec(draw, x, y, meta, "32", font32)
x, y = exec(draw, x, y, meta, "64", font64)

atlas.save("assets/font.png")

with open("assets/font.json", "w") as f:
    json.dump(meta, f, indent=2)
