from PIL import Image, ImageDraw, ImageFont
import json

FONT_PATH = "NotoSansJP.ttf"
SIZE = 32

ATLAS_W = 512
ATLAS_H = 512

ASCII_START = 32
ASCII_END = 126

font = ImageFont.truetype(FONT_PATH, SIZE)

atlas = Image.new("RGBA", (ATLAS_W, ATLAS_H), (0,0,0,0))
draw = ImageDraw.Draw(atlas)

x = 0
y = 0
row_h = 0

meta = {}

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

	meta[ch] = [x, y, w, h]

	x += w + 2
	row_h = max(row_h, h)

atlas.save("font_ascii_32.png")

with open("font_ascii_32.json","w") as f:
	json.dump(meta, f, indent=2)