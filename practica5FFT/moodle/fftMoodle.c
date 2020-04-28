#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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

datoCab *readCabecera(datoCab*,FILE*);
short *sampleSignal(FILE*,int,short*);
char *readPie(FILE*,char*,int);
void monoToStereo(datoCab*,FILE*);
void writePie(FILE*,char*,int);
void initFFT(char*,char*);
int isPowTwo(int);
datoCab *modCabecera(datoCab*,int,int);
short *completaMuestras(short*,int,int);
float *fft(float*,float*,int,float*);
void swapTwoValues(float*,float*);

int main(int argc, char **argv) {
  if(argc<3){
    printf("Error\nPlease Usage: %s <input-wav> <output-wav>",*argv);
    exit(EXIT_FAILURE);
  }
  initFFT(*(argv+1), *(argv+2));
}

datoCab *readCabecera(datoCab* c, FILE* read) {
  fread(c,44,1,read);
  return c;
}

short *sampleSignal(FILE* read, int N, short* signal) {
  fread(signal,sizeof(short)*N,1,read);
  return signal;
}

char *readPie(FILE* read, char* pie, int bytesPie) {
  fread(pie,sizeof(char)*bytesPie,1,read);
  return pie;
}

void monoToStereo(datoCab* c, FILE* write) {
  c->chunkSize=(c->chunkSize+c->subChunk2Size);
  c->numChannels=2;
  c->byteRate*=2;
  c->blockAlign*=2;
  c->subChunk2Size=(c->subChunk2Size*2);
  fwrite(c, 44, 1,write);
}

void writePie(FILE* write, char *pie, int bytesPie) {
  fwrite(pie,sizeof(char)*bytesPie, 1,write);
}

void initFFT(char *archivo, char *archivoSalida) {
  FILE *in=NULL,*out=NULL;
  short *signal=NULL,*muestras=NULL;
  datoCab *c = NULL;
  int N=0,no=0,NNuevo=0,i,bytesPie=0,l;
  char *pie=NULL;
  float *xr=NULL,*xi=NULL,*signalFFT=NULL;

  c=(datoCab*)malloc(sizeof(datoCab));

  in=fopen(archivo,"rb");
  if(in==NULL){
    printf("Error al abrir archivo: %s\n",archivo);
    exit(EXIT_FAILURE);
  }
  c=readCabecera(c,in);

  N=c->subChunk2Size/(c->bitsPerSample/8);
  bytesPie=(c->chunkSize)-(c->subChunk2Size)-36;
  pie=(char*)malloc(sizeof(char)*bytesPie);

  if(!isPowTwo(N)) {
    l=log2(N);
    l+=1;

    NNuevo=pow(2, l);

    c=modCabecera(c,N,NNuevo);

    signal=(short*)malloc(sizeof(short)*NNuevo);
    signal=sampleSignal(in, N, signal);

    signal=completaMuestras(signal, N, NNuevo);

    signalFFT=(float*)malloc(sizeof(float) * (NNuevo * 2));
    muestras=(short*)malloc(sizeof(short) * (NNuevo * 2));
    no=NNuevo;
  }else{
    signal=(short*)malloc(sizeof(short) * N);
    signal=sampleSignal(in, N, signal);

    signalFFT =(float*)malloc(sizeof(float) * (N * 2));
    muestras =(short*)malloc(sizeof(short) * (N * 2));
    no=N;
  }
  pie=readPie(in, pie, bytesPie);
  fclose(in);
  xr=(float*)malloc(sizeof(float)*no);
  xi=(float*)malloc(sizeof(float)*no);
  for(i=0;i<no;i++) {
    *(xr+i)=(float)*(signal+i);
    *(xi+i)=0.0;
  }
  signalFFT=fft(xr, xi, N, signalFFT);
  for(i=0;i<no*2;i++)
    *(muestras+i)=*(signalFFT+i);
  out=fopen(archivoSalida,"wb");
  if(out==NULL){
    printf("Error al abrir archivo: %s\n",archivo);
    exit(EXIT_FAILURE);
  }
  monoToStereo(c, out);

  fwrite(muestras,sizeof(short)*c->subChunk2Size, 1,out);
  fwrite(pie, sizeof(char)*bytesPie,1,out);
  fclose(out);

  free(c);
  free(signal);
  free(signalFFT);
  free(pie);
  free(muestras);
  free(xr);
  free(xi);
}

float *fft(float *xr, float *xi, int N, float *signalFFT) {
  int m, n, i, j, k, j1;
  float arg, s, c, tempr, tempi, w;
  m=log((float)N)/log(2.0);
  for(i=0;i<N;++i){
    j=0;
    for(k=0;k<m;++k)
      j=(j<<1)|(1&(i>>k));
    if(j<i){
      swapTwoValues(&*(xr+i),&*(xr+j));
      swapTwoValues(&*(xi+i),&*(xi+j));
    }
  }
  for(i=0;i<m;i++){
    n=w=pow(2.0,(float)i);
    w=PI/n;
    k=0;
    while(k<(N-1)){
      for(j=0;j<n;j++) {
        arg=-j*w;
        c=cos(arg);
        s=sin(arg);
        j1=k+j;
        tempr=*(xr+(j1+n))*c-*(xi+(j1+n))*s;
        tempi=*(xi+(j1+n))*c+*(xr+(j1+n))*s;
        *(xr+(j1+n))=*(xr+j1)-tempr;
        *(xi+(j1+n))=*(xi+j1)-tempi;
        *(xr+j1)=*(xr+j1)+tempr;
        *(xi+j1)=*(xi+j1)+tempi;
      }
      k+=2*n;
    }
  }
  arg=1.0/N;
  for(i=0;i<N;i++) {
    *(xr+i)*=arg;
    *(xi+i)*=arg;
    *(signalFFT+(2*i))=*(xr+i);
    *(signalFFT+((2*i)+1))=*(xi+i);
  }
  return signalFFT;
}

void swapTwoValues(float *a,float *b){
  float aux;
  aux=*a;
  *a=*b;
  *b=aux;
}

int isPowTwo(int N){
  return ((N!=0)&&((N&(N-1))==0));
}

datoCab *modCabecera(datoCab *c, int N, int NNuevo) {
  int m=NNuevo-N;
  c->chunkSize+=(m*2);
  c->subChunk2Size+=(m*2);
  return c;
}

short *completaMuestras(short *signal, int N, int NNuevo) {
  for(int i=N;i<NNuevo;i++)
    *(signal+i)=0;
  return signal;
}
