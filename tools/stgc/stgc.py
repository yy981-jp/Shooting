# stgc.py
from dataclasses import dataclass
from typing import List, Dict, Union
import sys


# ==========
# 命令定義
# ==========

@dataclass
class Instruction:
	op: str
	args: Union[list, dict, None] = None


@dataclass
class RepeatBlock:
	count: int
	body: list


@dataclass
class Call:
	target: str


# ==========
# パーサ本体
# ==========

class STGCompiler:
	def __init__(self, path: str):
		self.lines = open(path, encoding="utf-8").readlines()
		self.labels: Dict[str, list] = {}
		self.pos = 0

	def compile(self):
		self._collect_labels()
		return self._compile_label("main")

	# ----------------
	# 1パス目：ラベル収集
	# ----------------
	def _collect_labels(self):
		current = None
		for line in self.lines:
			# print(repr(line))
			line = line.strip()
			if not line or line.startswith("//"):
				continue
			if line.startswith(":"):
				current = line[1:]
				if current in self.labels:
					raise SyntaxError(f"duplicate label: {current}")
				self.labels[current] = []

		if "main" not in self.labels:
			raise SyntaxError("main label not found")

	# ----------------
	# ラベル単位でコンパイル
	# ----------------
	def _compile_label(self, name: str):
		instructions = []
		self.pos = 0

		# 対象ラベルまで進める
		while self.pos < len(self.lines):
			if self.lines[self.pos].strip() == f":{name}":
				self.pos += 1
				break
			self.pos += 1

		while self.pos < len(self.lines):
			line = self.lines[self.pos].strip()
			self.pos += 1

			if not line or line.startswith("//"):
				continue

			if line.startswith(":"):
				break

			tokens = line.split()

			cmd = tokens[0]

			if cmd == "wait":
				instructions.append(
					Instruction("wait", [int(tokens[1])])
				)

			elif cmd == "spawn":
				args = self._parse_kv(tokens[2:])
				args["type"] = tokens[1]
				instructions.append(
					Instruction("spawn", args)
				)

			elif cmd == "call":
				instructions.append(
					Call(tokens[1])
				)

			elif cmd == "repeat":
				count = int(tokens[1])
				body = self._parse_repeat()
				instructions.append(
					RepeatBlock(count, body)
				)

			elif cmd == "end":
				break

			else:
				raise SyntaxError(f"unknown command: {cmd}")

		return instructions

	# ----------------
	# repeat の中身
	# ----------------
	def _parse_repeat(self):
		body = []

		while self.pos < len(self.lines):
			line = self.lines[self.pos].strip()
			self.pos += 1

			if not line or line.startswith("//"):
				continue

			if line == "end":
				return body

			tokens = line.split()
			cmd = tokens[0]

			if cmd == "wait":
				body.append(Instruction("wait", [int(tokens[1])]))

			elif cmd == "spawn":
				args = self._parse_kv(tokens[2:])
				args["type"] = tokens[1]
				body.append(Instruction("spawn", args))

			elif cmd == "repeat":
				count = int(tokens[1])
				nested = self._parse_repeat()
				body.append(RepeatBlock(count, nested))

			else:
				raise SyntaxError(f"invalid in repeat: {cmd}")

		raise SyntaxError("repeat without end")

	# ----------------
	# key=value パース
	# ----------------
	def _parse_kv(self, items):
		result = {}
		for item in items:
			key, value = item.split("=")
			if value.isdigit():
				value = int(value)
			result[key] = value
		return result


# ==========
# 動作確認
# ==========

if __name__ == "__main__":
	compiler = STGCompiler(sys.argv[1])
	prog = compiler.compile()

	def dump(obj, indent=0):
		p = "  " * indent
		if isinstance(obj, list):
			for x in obj:
				dump(x, indent)
		elif isinstance(obj, Instruction):
			print(p, obj)
		elif isinstance(obj, RepeatBlock):
			print(p, f"Repeat {obj.count}")
			dump(obj.body, indent + 1)
		elif isinstance(obj, Call):
			print(p, f"Call {obj.target}")

	dump(prog)
