import struct
from defi import Instruction
from typing import List, Dict, Union, Any
import json
from pathlib import Path


opSizeTable: Dict[str,int] # op -> size(byte)
EVENT_TABLE: Dict[str,Any] = {}
FLAGS_TABLE: Dict[str,int] = {}
PARAM_TABLE: Dict[str,Any] ={}
ENTITY_TABLE: Dict[str,Any] ={}
ENTITY_MAP: Dict[str,int] ={}
enemyBezier_patterns: Dict[str,int] = {}

# loadJson
with open("../../assets/eventTable.json", "r", encoding="utf-8") as f:
    EVENT_TABLE = json.load(f)
    for idx, name in enumerate(EVENT_TABLE["flags"]):
        FLAGS_TABLE[name] = idx
    PARAM_TABLE = EVENT_TABLE["param"]
    patterns = PARAM_TABLE["enemyBezier"]["patterns"]
    enemyBezier_patterns = {name: i for i, name in enumerate(patterns)}
# from pprint import pprint

with open("../../assets/entity.def.json", "r", encoding="utf-8") as f:
    ENTITY_TABLE = json.load(f)
    ENTITY_MAP = {k: v["id"] for k, v in ENTITY_TABLE.items()}
    # pprint(ENTITY_TABLE)
    # pprint(ENTITY_MAP)


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
        entityType:str = cmd.args["entityType"]
        if (entityType == "enemyBezier"):
            pattern = int(
                enemyBezier_patterns.get(
                    cmd.args["pattern"],
                    enemyBezier_patterns["default"]
                )
            ) if "pattern" in cmd.args else 0
            code+= struct.pack(
                "<H H H H I",
                int(ENTITY_MAP[entityType]),
                int(cmd.args["x"]), int(cmd.args["y"]),
                pattern,
                int(cmd.args.get("duration", 5000))
            )
        else:
            raise SyntaxError("spawn: 未知の敵")

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

