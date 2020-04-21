cabeceraWAV=cabeceraWAV
main=main
compile-all: C/${cabeceraWAV}.c C/${main}.c
	gcc -c C/${cabeceraWAV}.c -lm -o O/${cabeceraWAV}.o -I H/
	gcc C/${main}.c O/${cabeceraWAV}.o -lm -o ${main} -I H/
