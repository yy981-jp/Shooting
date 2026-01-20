from defi import Instruction
from typing import Dict, Union


FPS: int = 60
MAX_NEST: int = 16


# ==========
# パーサ本体
# ==========

class STGCompiler:
	def __init__(self, path: str):
		raw = open(path, encoding="utf-8").read()
		raw = self._remove_comments(raw)
		self.lines = raw.splitlines()
		self.labels: Dict[str, str] = {}
		self.pc = 0

	def compile(self):
		instructions = []

		self._collect_labels()
		for label in self.labels:
			instructions.extend(self._compile_label(label))
		
		return instructions, self.labels["main"], self.pc

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
				self.labels[current] = None

		if "main" not in self.labels:
			raise SyntaxError("main label not found")

	# ----------------
	# ラベル単位でコンパイル
	# ----------------
	def _compile_label(self, name:str):
		self.labels[name] = self.pc
		instructions = []
		pos = 0
		loopStack = []

		# 対象ラベルまで進める
		while pos < len(self.lines):
			if self.lines[pos].strip() == f":{name}":
				pos += 1
				break
			pos += 1

		while pos < len(self.lines):
			line = self.lines[pos].strip()
			pos += 1

			if line.startswith(":"):
				break # 関数終了

			tokens = line.split()
			if not tokens:
				continue

			cmd = tokens[0]
			self.pc += 1

			if cmd == "wait":
				instructions.append(
					Instruction("wait", self._parse_second(tokens[1]))
				)

			elif cmd == "waitUntil":
				instructions.append(
					Instruction("waitUntil",tokens[1])
				)

			elif cmd == "spawn":
				args = self._parse_kv(tokens[2:])
				args["entityType"] = tokens[1]
				instructions.append(
					Instruction("spawn", args)
				)

			elif cmd == "call":
				instructions.append(
					Instruction("call", self.labels[tokens[1]])
				)

			elif cmd == "repeat":
				instructions.append(
					Instruction("repeat", int(tokens[1]))
				)
				if (len(loopStack) >= MAX_NEST):
					raise SyntaxError(f"深さ{MAX_NEST}以上のネストは許容できません")
				loopStack.append(self.pc)

			elif cmd == "repeatUntil":
				instructions.append(
					Instruction("repeatUntil", tokens[1])
				)
				if (len(loopStack) >= MAX_NEST):
					raise SyntaxError(f"深さ{MAX_NEST}以上のネストは許容できません")
				loopStack.append(self.pc)

			elif cmd == "end":
				if not loopStack:
					raise SyntaxError("repeat に対応する end がありません")
				loopStack.pop()
				instructions.append(
					Instruction("end")
				)

			else:
				raise SyntaxError(f"unknown command: {cmd}")
	
		if loopStack:
			raise SyntaxError("repeat に対応する end が足りません")

		if (name == "main"):
			instructions.append(Instruction("shutdown"))
		return instructions


	def _remove_comments(self, text: str) -> str:
		result = []
		i = 0
		n = len(text)
		in_block_comment = False

		while i < n:
			# ブロックコメント中
			if in_block_comment:
				if text[i:i+2] == "*/":
					in_block_comment = False
					i += 2
				else:
					i += 1
				continue

			# 行コメント //
			if text[i:i+2] == "//":
				# 行末までスキップ
				while i < n and text[i] != "\n":
					i += 1
				continue

			# ブロックコメント開始 /*
			if text[i:i+2] == "/*":
				in_block_comment = True
				i += 2
				continue

			# 通常文字
			result.append(text[i])
			i += 1

		if in_block_comment:
			raise SyntaxError("ブロックコメントが閉じられていません")

		return "".join(result)
	def _parse_value(self, value: str):
		v = value.strip()

		# bool
		if v.lower() == "true":
			return True
		if v.lower() == "false":
			return False

		# int
		try:
			return int(v)
		except ValueError:
			pass

		# float
		try:
			return float(v)
		except ValueError:
			pass

		# string
		return v
	def _parse_kv(self, items):
		result = {}
		for item in items:
			key, value = item.split("=", 1)
			result[key] = self._parse_value(value)
		return result
	def _parse_second(self, value: str):
		tick: int = 0
		if value.endswith("ms"):
			tick = int(value[:-2]) *FPS *1000
		elif value.endswith("s"):
			tick = int(value[:-1]) *FPS
		else:
			tick = int(value)
		return tick
