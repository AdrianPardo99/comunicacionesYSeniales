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


datoCab *readCabecera(datoCab*,FILE*);
short *sampleSignal(FILE*,int,short*);
char *readPie(FILE*,int,char*);
void copiaPie(FILE*,int,char*);
short *channel1(short*,short*,int);
short *channel2(short*,short*,int);
double *idft(short*,short*,double*,int);
void initIDFT(char*,char*);

int main(int argc, char *argv[]) {
  if(argc<3){
    printf("Error\nPlease Usage: %s <input-wav> <output-wav>",*argv);
    exit(EXIT_FAILURE);
  }
  initIDFT(*(argv+1),*(argv+2));
  return 0;
}
datoCab *readCabecera(datoCab *cab,FILE *read){
  fread(cab,44,1,read);
  return cab;
}

short *sampleSignal(FILE *read,int N,short *arr){
  fread(arr,sizeof(short)*N,1,read);
  return arr;
}

char *readPie(FILE *read,int BP,char *arr){
  fread(arr,sizeof(char)*BP,1,read);
  return arr;
}

void copiaPie(FILE *write,int N,char *arr){
  fwrite(arr,sizeof(char)*N,1,write);
}

short *channel1(short *senial,short *canal1,int N){
  int i;
  for(i=0;i<N;i++){
    *(canal1+i)=*(senial+(2*i));
  }
  return canal1;
}

short *channel2(short *senial,short *canal2,int N){
  int i;
  for(i=0;i<N;i++){
    *(canal2+i)=*(senial+((2*i)+1));
  }
  return canal2;
}

double *idft(short *canal1,short *canal2,double *senialIDFT,int N){
  int n=0,k=0;
  double cons=(2*PI)/(N/2);
  for(n=0;n<N/2;n++){
    *(senialIDFT+(2*n))=0;
    *(senialIDFT+((2*n)+1))=0;
    for(k=0;k<N/2;k++){
      *(senialIDFT+(2*n))+=(*(canal1+k)*cos(cons*k*n))-(*(canal2+k)*sin(cons*k*n));
    }
    if(*(senialIDFT+(2*n))>32767){
      *(senialIDFT+(2*n))=32767;
    }else if(*(senialIDFT+(2*n))<-32768){
      *(senialIDFT+(2*n))=-32768;
    }
    for(k=0;k<N/2;k++){
      *(senialIDFT+((2*n)+1))+=(*(canal1+k)*sin(cons*k*n))+(*(canal2+k)*cos(cons*k*n));
    }
    if(*(senialIDFT+((2*n)+1))>32767){
      *(senialIDFT+((2*n)+1))=32767;
    }else if(*(senialIDFT+((2*n)+1))<-32768){
      *(senialIDFT+((2*n)+1))=-32768;
    }
  }
  return senialIDFT;
}

void initIDFT(char *in,char *out){
  FILE *read,*write;
  datoCab *cab;
  int N,bP,i;
  short *canal1,*canal2,*senial,*muestras;
  char *pie;
  double *senialIDFT;
  read=fopen(in,"rb");
  if(read==NULL){
    printf("Error al abrir archivo: %s\n",in);
    exit(EXIT_FAILURE);
  }
  cab=(datoCab*)malloc(sizeof(datoCab));
  cab=readCabecera(cab,read);
  N=cab->subChunk2Size/(cab->bitsPerSample/8);
  senial=(short*)malloc(sizeof(short)*N);
  senial=sampleSignal(read,N,senial);
  bP=cab->chunkSize-cab->subChunk2Size-36;
  pie=(char*)malloc(sizeof(char)*bP);
  pie=readPie(read,bP,pie);
  fclose(read);
  canal1=(short*)malloc(sizeof(short)*(N/2));
  canal2=(short*)malloc(sizeof(short)*(N/2));
  canal1=channel1(senial,canal1,N/2);
  canal2=channel2(senial,canal2,N/2);
  senialIDFT=(double*)malloc(sizeof(double)*N);
  senialIDFT=idft(canal1,canal2,senialIDFT,N);
  muestras=(short*)malloc(sizeof(short)*N);

  for(i=0;i<N;i++){
    *(muestras+i)=*(senialIDFT+i);
  }
  write=fopen(out,"wb");
  if(write==NULL){
    printf("Error archivo incorrecto\n");
    free(senial);
    free(pie);
    free(canal1);
    free(canal2);
    free(senialIDFT);
    free(muestras);
    free(cab);
    exit(EXIT_FAILURE);
  }
  fwrite(cab,44,1,write);
  fwrite(muestras,sizeof(short)*cab->subChunk2Size,1,write);
  fwrite(pie,sizeof(char)*bP,1,write);
  free(senial);
  free(pie);
  free(canal1);
  free(canal2);
  free(senialIDFT);
  free(muestras);
  free(cab);
  fclose(write);

}
