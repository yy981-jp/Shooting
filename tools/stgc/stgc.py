# stgc.py
import sys

from parse import STGCompiler
from convert import write


from pprint import pprint

if __name__ == "__main__":
	compiler = STGCompiler(sys.argv[1])
	prog, mainAddr, endAddr = compiler.compile()
	write(prog,mainAddr,endAddr,sys.argv[1])

	# print(f"CompiledSize: {len(prog)}")
	pprint(prog)
