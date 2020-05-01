/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 30-04-2020 */
#include "color.h"
#define PI 3.1415926535

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

datoCab *readCabecera(datoCab *, FILE *);
short *sampleSignal(FILE *, int, short *);
char *readPie(FILE *, char*, int);
void cabToStereo(datoCab *, FILE *);
void writePie(FILE *, char *, int);
short *channel1(short *, short *, int);
short *channel2(short *, short *, int);
float *ifft(float *xr, float *xi, int numeroMuestras, float *signalIFFT);
void swap(float *a, float *b);
void initIFFT(char *, char *);
