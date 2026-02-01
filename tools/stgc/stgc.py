# stgc.py
import sys

from parse import STGCompiler
from convert import write


from pprint import pprint

if __name__ == "__main__":
	compiler = STGCompiler(sys.argv[1])
	prog, mainAddr, endAddr = compiler.compile()
	write(prog,mainAddr,endAddr,sys.argv[1])

	if len(sys.argv)==3 and sys.argv[2] == "debug":
		print(f"CompiledSize: {len(prog)}")
		print(f"mainAddr: {mainAddr}")
		pprint(prog)
