import struct
from defi import Instruction
from typing import List, Dict, Union, Any, Optional
import json
from pathlib import Path
import re
from pprint import pprint
import os

# 従った型マッピング
TYPE_MAP = {
	"s8": "b",
	"u8": "B",
	"s16": "h",
	"u16": "H",
	"s32": "i",
	"u32": "I",
	"f": "f",
}

def load_entity_defs(xmac_path: str) -> Dict[str, List[tuple]]:
	"""XMacro ファイルから entity 定義を読み取る
	Returns:
		{entity_name: [(type, name, default), ...], ...}
	"""
	entity_defs = {}
	
	with open(xmac_path, encoding="utf-8") as f:
		text = f.read()
	
	# #define 行や #undef 行を除外（C preprocessor 対応）
	lines = text.split('\n')
	filtered_lines = []
	for line in lines:
		line = line.strip()
		if not line.startswith('#'):
			filtered_lines.append(line)
	text = '\n'.join(filtered_lines)
	
	# 括弧のバランスを考慮した ENTITY(...) ブロック抽出
	i = 0
	while i < len(text):
		# ENTITY( を探す
		idx = text.find('ENTITY(', i)
		if idx == -1:
			break
		
		# Entity 名を取得
		paren_start = idx + 7  # 'ENTITY('.length
		comma_idx = text.find(',', paren_start)
		if comma_idx == -1:
			break
		
		entity_name = text[paren_start:comma_idx].strip()
		
		# 対応する ) を探す（括弧のバランスを考慮）
		depth = 1
		j = comma_idx + 1
		fields_start = j
		while j < len(text) and depth > 0:
			if text[j] == '(':
				depth += 1
			elif text[j] == ')':
				depth -= 1
			j += 1
		
		if depth == 0:
			fields_text = text[fields_start:j-1]
			
			# FIELD(...) を抽出
			fields = []
			field_pattern = re.compile(r'FIELD\((\w+),\s*(\w+)(?:\s*,\s*([^)]*))?\)')
			
			for field_match in field_pattern.finditer(fields_text):
				field_type = field_match.group(1)
				field_name = field_match.group(2)
				default_str = field_match.group(3)
				
				# default を処理
				default = None
				if default_str:
					try:
						default = int(default_str)
					except ValueError:
						default = default_str
				
				fields.append((field_type, field_name, default))
			
			entity_defs[entity_name] = fields
		
		i = j
	
	return entity_defs

def load_entity_ids(xmac_path: str) -> Dict[str, int]:
	"""XMacro ファイルから entity ID を取得（定義順）"""
	entity_ids = {}
	entity_defs = load_entity_defs(xmac_path)
	
	for idx, entity_name in enumerate(entity_defs.keys()):
		entity_ids[entity_name] = idx
	
	return entity_ids

def make_struct_format(fields: List[tuple]) -> str:
	fmt = "<"
	for field_type, _, _ in fields:
		fmt += TYPE_MAP[field_type]
	return fmt

def parse_second_value(value: str) -> int:
	"""秒文字列を ms の int に変換"""
	if isinstance(value, int):
		return value
	value = str(value).strip()
	if value.endswith("ms"):
		return int(value[:-2])
	elif value.endswith("s"):
		return int(value[:-1]) * 1000
	else:
		return int(value)


# XMacro から entity 定義と ID を読み込む
current_dir = os.path.dirname(os.path.abspath(__file__))
entity_defs_path = os.path.join(current_dir, "..", "shared", "stgEntity.def")

ENTITY_DEFS: Dict[str, List[tuple]] = load_entity_defs(entity_defs_path)
ENTITY_IDS: Dict[str, int] = load_entity_ids(entity_defs_path)

def pack_spawn_data(entity_type: str, args: Dict[str, Any]) -> bytes:
	"""spawn コマンドのエンティティデータをバイナリ化"""
	if entity_type not in ENTITY_DEFS:
		raise SyntaxError(f"unknown entity: {entity_type}")
	
	fields = ENTITY_DEFS[entity_type]
	fmt = make_struct_format(fields)
	
	values = []
	for field_type, field_name, default in fields:
		if field_name in args:
			value = args[field_name]
		elif default is not None:
			value = default
		else:
			raise SyntaxError(f"spawn {entity_type}: required field '{field_name}' not provided")
		
		# duration フィールドの場合、秒を ms に変換
		if field_name == "duration":
			value = parse_second_value(value)
		
		# pattern フィールドの場合、eventTable.json から ID を取得
		if field_name == "pattern" and isinstance(value, str):
			if entity_type in EVENT_TABLE.get("param", {}) and "patterns" in EVENT_TABLE["param"][entity_type]:
				patterns = EVENT_TABLE["param"][entity_type]["patterns"]
				if value in patterns:
					value = patterns.index(value)
				else:
					raise SyntaxError(f"unknown pattern '{value}' for {entity_type}")
		
		# 型変換（文字列値を処理）
		if isinstance(value, str):
			# 文字列を型に応じて変換
			if field_type in ["int8", "uint8", "int16", "uint16", "int32", "uint32"]:
				# 文字列が数値な場合のみ変換、そうでなければデフォルト値を使う
				try:
					value = int(value)
				except ValueError:
					# 数値に変換できない場合、デフォルト値を使う
					value = default if default is not None else 0
			elif field_type == "float":
				try:
					value = float(value)
				except ValueError:
					value = default if default is not None else 0.0
		else:
			# 非文字列の場合は型変換
			if field_type in ["int8", "uint8", "int16", "uint16", "int32", "uint32"]:
				value = int(value)
			elif field_type == "float":
				value = float(value)
		
		values.append(value)
	
	return struct.pack(fmt, *values)




# Flag システムは別途初期化（event table から読み込み）
EVENT_TABLE: Dict[str,Any] = {}
FLAGS_TABLE: Dict[str,int] = {}

try:
	assets_dir = os.path.join(current_dir, "..", "assets")
	event_table_path = os.path.join(assets_dir, "eventTable.json")
	with open(event_table_path, "r", encoding="utf-8") as f:
		EVENT_TABLE = json.load(f)
		for idx, name in enumerate(EVENT_TABLE["flags"]):
			FLAGS_TABLE[name] = idx
except Exception:
	pass


def packParam(cmd:Instruction):
	code = bytearray()
	if (cmd.op == "shutdown"):
		opCode = 0
		code+= struct.pack("<B",opCode) #uint8

	elif (cmd.op == "wait"):
		opCode = 1
		code+= struct.pack("<B",opCode)
		tick:int = cmd.args
		code+= struct.pack("<I",tick) #uint32

	elif (cmd.op == "waitUntil"):
		opCode = 2
		code+= struct.pack("<B",opCode)
		flagID:int = FLAGS_TABLE[cmd.args]
		code+= struct.pack("<H",flagID) #uint16

	elif (cmd.op == "spawn"):
		opCode = 3
		code+= struct.pack("<B",opCode)
		
		entity_type = cmd.args["entityType"]
		entity_id = ENTITY_IDS[entity_type]
		
		code+= struct.pack("<H", entity_id) #uint16: STGEntityID
		code+= pack_spawn_data(entity_type, cmd.args) #entity struct

	elif (cmd.op == "call"):
		opCode = 4
		code+= struct.pack("<B",opCode)
		labelAddr:int = cmd.args
		code+= struct.pack("<I",labelAddr)
	
	elif (cmd.op == "ret"):
		opCode = 5
		code+= struct.pack("<B",opCode)

	elif (cmd.op == "repeat"):
		opCode = 6
		code+= struct.pack("<B",opCode)
		count:int = cmd.args
		code+= struct.pack("<H",count)
	
	elif (cmd.op == "repeatUntil"):
		opCode = 7
		code+= struct.pack("<B",opCode)
		flagID:int = FLAGS_TABLE[cmd.args]
		code+= struct.pack("<H",flagID)

	elif (cmd.op == "end"):
		opCode = 8
		code+= struct.pack("<B",opCode)

	return code


def write(inss:list[Instruction], entry_pc:int, endAddr:int, filename:str):
	# Header
	code = struct.pack(
		"<8s H I I",
		b"y9STGBin",
		1,
		entry_pc,
		endAddr
	)
	
	# Body
	for ins in inss:
		code+= packParam(ins)
	
	with open(Path(filename).name+".dat", "wb") as f:
		f.write(code)

