header=cabeceraWAV
source=main

comp: C/${header}.c C/${source}.c
	gcc -c C/${header}.c -lm -o O/${header}.o -I H/
	gcc C/${source}.c O/${header}.o -lm -o ${source} -I H/
