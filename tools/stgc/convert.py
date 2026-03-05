import struct
from defi import Instruction
from typing import List, Dict, Union, Any
import json
from pathlib import Path
import re

from pprint import pprint

TYPE_MAP = {
	"u8":  "B",
	"s8":  "b",
	"u16": "H",
	"s16": "h",
	"u32": "I",
	"s32": "i",
	"f32": "f",
}

import re

def load_spawn_defs(path):
	spawns = {}

	with open(path, encoding="utf-8") as f:
		text = f.read()

	blocks = re.findall(r'X\((.*?)\)', text, re.S)

	field_pattern = re.compile(r'(\w+)\s+(\w+)')

	for block in blocks:
		lines = field_pattern.findall(block)

		name = block.split(",",1)[0].strip()
		fields = [(t,n) for t,n in lines]

		spawns[name] = fields
		
	return spawns

def make_struct_format(fields):
	fmt = "<"
	for t, _ in fields:
		fmt += TYPE_MAP[t]
	return fmt

SPAWN_DEFS: Dict[str,list[tuple[str, str]]] = load_spawn_defs("../../common/spawn.def")

def pack_spawn(entity:str, args):
    fields: list[tuple[str, str]] = SPAWN_DEFS[entity]
    
    if not fields:
        raise SyntaxError("spawn: 未知の敵")

    fmt = make_struct_format(fields)

    values = []
    for t, name in fields:
        values.append(int(args[name]))

    return struct.pack(fmt, *values)


opSizeTable: Dict[str,int] # op -> size(byte)
EVENT_TABLE: Dict[str,Any] = {}
FLAGS_TABLE: Dict[str,int] = {}
PARAM_TABLE: Dict[str,Any] ={}
ENTITY_TABLE: Dict[str,int] ={}
enemyBezier_patterns: Dict[str,int] = {}

# loadJson
with open("../../assets/eventTable.json", "r", encoding="utf-8") as f:
    EVENT_TABLE = json.load(f)
    for idx, name in enumerate(EVENT_TABLE["flags"]):
        FLAGS_TABLE[name] = idx
    PARAM_TABLE = EVENT_TABLE["param"]
    patterns = PARAM_TABLE["enemyBezier"]["patterns"]
    enemyBezier_patterns = {name: i for i, name in enumerate(patterns)}

with open("../../assets/entityTable.def", "r", encoding="utf-8") as f:
    index = 0
    for line in f:
        # コメント除去
        line = line.split("//")[0].strip()
        if not line:
            continue

        m = re.match(r"X\((\w+)\)", line)
        if m:
            name = m.group(1)
            ENTITY_TABLE[name] = index
            index += 1
    # pprint(ENTITY_TABLE)

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
        code+= pack_spawn(cmd.op,cmd.args)

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

