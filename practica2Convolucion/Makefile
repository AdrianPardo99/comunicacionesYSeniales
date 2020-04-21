cabecera=cabeceraWAV
source=main

comp: C/${cabecera}.c
	gcc -c C/${cabecera}.c -lm -o O/${cabecera}.o -I H/
	gcc C/${source}.c O/${cabecera}.o -lm -o ${source} -I H/
