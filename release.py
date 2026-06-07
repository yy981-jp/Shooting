from pathlib import Path
import subprocess
import shutil
import re
import os
import stat

RELEASE_DIR = Path("release")
ASSETS_DIR = Path("build/assets")

TARGETS = [
	Path("build/st.exe"),
]

DLL_PATTERN = re.compile(r"=>\s+([^\s]+\.dll)", re.IGNORECASE)


def collect_dlls(exe: Path) -> set[Path]:
	result = subprocess.run(
		["ntldd", "-R", str(exe)],
		capture_output=True,
		text=True,
		check=True
	)

	dlls = set()

	for line in result.stdout.splitlines():
		match = DLL_PATTERN.search(line)

		if not match:
			continue

		dll = Path(match.group(1))

		if dll.exists():
			dlls.add(dll.resolve())

	return dlls


def copy_file(src: Path, dst_dir: Path):
	dst = dst_dir / src.name
	os.chmod(dst, stat.S_IWRITE)
	shutil.copy2(src, dst)
	# print(f"COPY {src.name}")


def main():
	RELEASE_DIR.mkdir(exist_ok=True)

	all_dlls = set()

	for exe in TARGETS:
		if not exe.exists():
			print(f"SKIP {exe}")
			continue

		copy_file(exe, RELEASE_DIR)

		try:
			all_dlls |= collect_dlls(exe)

		except subprocess.CalledProcessError as e:
			print(f"ntldd failed: {exe}")
			print(e.stdout)
			print(e.stderr)

	for dll in sorted(all_dlls):
		copy_file(dll, RELEASE_DIR)

	if ASSETS_DIR.exists():
		dst = RELEASE_DIR / "assets"

		if dst.exists():
			shutil.rmtree(dst)

		shutil.copytree(ASSETS_DIR, dst)

		print("COPY assets")

	# print()
	print(f"DLL count : {len(all_dlls)}")

	# zip
	# shutil.make_archive(
	# 	"release",
	# 	"zip",
	# 	RELEASE_DIR
	# )

	print("Done")


if __name__ == "__main__":
	main()
