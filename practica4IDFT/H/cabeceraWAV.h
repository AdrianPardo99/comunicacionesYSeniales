#include "color.h"
/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 24-04-2020 */
#include "color.h"


typedef struct datos{
  char chunkID[4];
  int chunkSize;
  char format[4];
  /* Aqui comienza el primer subchunk 'fmt' */
  char subChunk1Id[4];
  int subChunk1Size;
  short int audioFormat;
  short int numChannels;		/* Numero de canales */
	int sampleRate;			/* Frecuencia de muestreo */
	int byteRate;			/* (SampleRate * Numero canales * Bits per Sample) / 8 */
	short int blockAlign;		/* Bytes por muestra */
	short int bitsPerSample;	/* 8 bits, 16 bits, etc. */

	/* Aqui comienza el segundo subchunk 'data' */
  char subChunk2Id[4];	/* Contiene 'data' */
  int subChunk2Size;		/* Numero de bytes en los datos */
}datoCab;

datoCab *readCabecera(datoCab*,FILE*);
short *sampleSignal(FILE*,int,short*);
char *readPie(FILE*,int,char*);
void copiaPie(FILE*,int,char*);
short *channel1(short*,short*,int);
short *channel2(short*,short*,int);
double *idft(short*,short*,double*,int);
void initIDFT(char*,char*);
