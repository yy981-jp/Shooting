from PIL import Image, ImageDraw, ImageFont
import json

FONT_PATH = "NotoSansJP.ttf"

ATLAS_W = 1024
ATLAS_H = 512

ASCII_START = 32
ASCII_END = 126
# 95 array


def exec(draw, x, y, row_h, meta, size: str, font):

	if size not in meta:
		meta[size] = {}

	for c in range(ASCII_START, ASCII_END + 1):
		ch = chr(c)

		bbox = font.getbbox(ch)
		w = bbox[2] - bbox[0]
		h = bbox[3] - bbox[1]

		if x + w >= ATLAS_W:
			x = 0
			y += row_h + 2
			row_h = 0

		draw.text((x - bbox[0], y - bbox[1]), ch, font=font, fill=(255,255,255,255))

		meta[size][str(ord(ch))] = [x, y, w, h]

		x += w + 2
		row_h = max(row_h, h)

	return x, y, row_h

font16 = ImageFont.truetype(FONT_PATH, 16)
font32 = ImageFont.truetype(FONT_PATH, 32)
font64 = ImageFont.truetype(FONT_PATH, 64)

atlas = Image.new("RGBA", (ATLAS_W, ATLAS_H), (0,0,0,0))
draw = ImageDraw.Draw(atlas)

x = 0
y = 0
row_h = 0

meta = {}

x, y, row_h = exec(draw, x, y, row_h, meta, "16", font16)
x, y, row_h = exec(draw, x, y, row_h, meta, "32", font32)
x, y, row_h = exec(draw, x, y, row_h, meta, "64", font64)

atlas.save("font.png")

with open("font.json","w") as f:
	json.dump(meta, f, indent=2)