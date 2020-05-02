#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

short absShort(short);
datoCab *readCabecera(datoCab *, FILE *);
short *sampleSignal(FILE *, int, short *);
double *fdt(int,short*,double*);
void dtmf(short*,double);
void initDMTF(char *);

int main(int argc, char **argv) {
  if(argc<2){
    printf("Error\nPlease Usage: %s <input-wav>",*argv);
    exit(EXIT_FAILURE);
  }
  initDMTF(*(argv+1));
}

short absShort(short a){
  return (a<0)?(-a):(a);
}

datoCab *readCabecera(datoCab* c, FILE* fileRead) {
  fread(c,44,1,fileRead);
  return c;
}

short *sampleSignal(FILE* fileRead,int N,short* signal) {
  fread(signal,N*sizeof(short),1,fileRead);
  return signal;
}

double *fdt(int N,short *signal,double *signalFDT){
  double *xr=(double*)malloc(sizeof(double)*N),
    *xi=(double*)malloc(sizeof(double)*N);
  int k,n;
  for(k=0;k<N;k++){
    *(xr+k)=*(xi+k)=0;
    for(n=0;n<N;n++){
      *(xr+k)+=*(signal+n)*cos((2*PI*k*n)/N);
      *(xi+k)+=-*(signal+n)*sin((2*PI*k*n)/N);
    }
    *(xr+k)/=N;
    *(xi+k)/=N;
    *(signalFDT+k)=sqrt(pow(*(xr+k),2)+pow(*(xi+k),2));
  }
  free(xr);
  free(xi);
  return signalFDT;
}

void dtmf(short *samples,double t){
  int fDTMF[8]={697, 770, 852, 941, 1209, 1336, 1477, 1633},pos[8];
  char keyboard[4][4]
    ={{'1','2','3','A'},
      {'4','5','6','B'},
      {'7','8','9','C'},
      {'*','0','#','D'}};
  short f,i;
  int p;
  for(i=0;i<8;i++){
    p=(int)(*(fDTMF+i)*t);
    f=absShort(*(samples+p));
    if(f>500){
      *(pos+i)=1;
    }else{
      *(pos+i)=0;
    }
  }
  for(i=0;i<4;i++){
    if(*(pos+i)){
      if(*(pos+4)){
        printf("%c",*(*(keyboard+i)));
      }
      if(*(pos+5)){
        printf("%c",*(*(keyboard+i)+1));
      }
      if(*(pos+6)){
        printf("%c",*(*(keyboard+i)+2));
      }
      if(*(pos+7)){
        printf("%c",*(*(keyboard+i)+3));
      }
    }
  }
  printf("\n");
}

void initDMTF(char *inputRead) {
  FILE *fileRead;
  short *signal,*muestras;
  datoCab *c = NULL;
  int N = 0,fm,i;
  double *signalDMTF,t;

  c=(datoCab*)malloc(sizeof(datoCab));
  fileRead=fopen(inputRead, "rb");
  if(fileRead==NULL){
    printf("Error al abrir archivo: %s\n",inputRead);
    exit(EXIT_FAILURE);
  }
  c=readCabecera(c, fileRead);
  N=c->subChunk2Size/(c->bitsPerSample/8);
  fm=c->sampleRate;
  signal=(short*)malloc(sizeof(short)*N);
  signalDMTF=(double*)malloc(sizeof(double)*N);
  muestras=(short*)malloc(sizeof(short)*N);

  signal=sampleSignal(fileRead, N, signal);

  fclose(fileRead);
  t=(double)(((double)N)/(double)fm);
  signalDMTF=fdt(N,signal,signalDMTF);
  for(i=0;i<N;i++){
    *(muestras+i)=*(signalDMTF+i);
  }
  dtmf(muestras,t);

  free(c);
  free(signal);
  free(signalDMTF);
  free(muestras);
}
