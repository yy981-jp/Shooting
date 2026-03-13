from rectpack import newPacker
from PIL import Image, ImageSequence
import os
import json

INPUT = "assets/image"
ATLAS_W = 2048
ATLAS_H = 2048

images = {}
rects = []

# spriteごとのframe管理
groups = {}

for f in os.listdir(INPUT):

	path = os.path.join(INPUT, f)

	# --- PNG ---
	if f.endswith(".png"):

		img = Image.open(path).convert("RGBA")

		key = f
		sprite = os.path.splitext(f)[0]

		images[key] = img
		rects.append((img.width, img.height, key))

		groups[sprite] = {}
		groups[sprite]["frame"] = []

		groups[sprite]["frame"].append(key)

	# --- GIF ---
	elif f.endswith(".gif"):

		sprite = os.path.splitext(f)[0]
		groups[sprite] = {}
		groups[sprite]["frame"] = []

		gif = Image.open(path)

		# spf
		groups[sprite]["spf"] = gif.info.get("duration", 0) / 1000

		i = 0
		for frame in ImageSequence.Iterator(gif):

			img = frame.convert("RGBA")

			key = f"{sprite}#{i}"

			images[key] = img
			rects.append((img.width, img.height, key))

			groups[sprite]["frame"].append(key)

			i += 1


packer = newPacker(rotation=False)

for r in rects:
	packer.add_rect(*r)

packer.add_bin(ATLAS_W, ATLAS_H)
packer.pack()

atlas = Image.new("RGBA", (ATLAS_W, ATLAS_H))

meta = {}

for rect in packer.rect_list():

	bin, x, y, w, h, name = rect

	img = images[name]
	atlas.paste(img, (x, y))

	meta[name] = [x, y, w, h]


# --- sprite単位json生成 ---
result = {}
result = {
	"version": 1 
}

for sprite, frames in groups.items():

    result[sprite] = {}
    result[sprite]["frame"] = []

    for key in frames["frame"]:
        result[sprite]["frame"].append(meta[key])
	
    if "spf" in frames:
        result[sprite]["spf"] = frames["spf"]

atlas.save("assets/atlas.png")

with open("assets/atlas.json", "w") as f:
	json.dump(result, f, indent=2)


# --- XMacro生成 ---

def_name = "assets/sprite.def"

# sprite名はgroupsのキーを使う
names = sorted(groups.keys())

with open(def_name, "w") as f:
	for n in names:
		f.write(f"X({n})\n")
