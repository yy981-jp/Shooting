from dataclasses import dataclass
from typing import List, Dict, Union


@dataclass
class Instruction:
	op: str
	args: Union[list, dict, None] = None
