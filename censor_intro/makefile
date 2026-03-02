
# Change paths to compiler, vice, debugger and exomizer to point to on your local filesystem

CC=oscar64
RDEBUG=retrodebug
EXOMIZER=exomizer
VICE=x64sc
INC=~/c64/oscar64/include
OBJ = main.prg
EXO = main_exo.prg

all: prg
#  	$(RDEBUG) -crt main.crt

prg: main.c 
	$(CC) -i=$(INC) -n -O3 -xz -tf=prg -tm=c64 $< -o=$(OBJ)

exo: all
	$(EXOMIZER) sfx 0x080d main.prg -o $(EXO)

run: prg
	$(VICE) -autostartprgmode 1 -moncommands main.lbl $(OBJ)

debug: main.c
	$(CC) -i=$(INC) -n -O3 -g -D _DEBUG_=true -xz -tf=prg -tm=c64 $< -o=$(OBJ)
	$(RDEBUG) $(OBJ)

clean:
	rm -f main.prg $(EXO) *.lbl *.int *.asm *.map *.dbj *.out *.crt *.sym


.PHONY: clean prg exo

