/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 30-04-2020 */
#include "cabeceraWAV.h"

datoCab *readCabecera(datoCab* c, FILE* fileRead) {
  fread(c,44,1,fileRead);
  return c;
}

short *sampleSignal(FILE* fileRead,int numeroMuestras,short* signal) {
  fread(signal,numeroMuestras*sizeof(short),1,fileRead);
  return signal;
}

short *channel1(short *signal,short *canal1,int numeroMuestras) {
  for(int i=0;i<(numeroMuestras);i++)
    *(canal1+i)=*(signal+(2*i));
  return canal1;
}

short *channel2(short *signal, short *canal2, int numeroMuestras) {
  for(int i=0;i<(numeroMuestras);i++)
    *(canal2+i) = *(signal+((2 * i)+1));
  return canal2;
}

char *readPie(FILE* fileRead, char* pie, int bytesPie) {
  fread(pie,sizeof(char)*bytesPie,1,fileRead);
  return pie;
}

void writePie(FILE* fileWrite, char *pie, int bytesPie) {
  fwrite(pie,sizeof(char)*bytesPie,1,fileWrite);
}


float *ifft(float *xr, float *xi, int numeroMuestras, float *signalIFFT) {
  int m, n, i, j, k, j1;
  float arg, s, c, tempr, tempi, w;
  m=log((float)numeroMuestras)/log(2.0);
  for(i=0;i<numeroMuestras;++i){
    j=0;
    for(k=0;k<m;++k)
      j=(j<<1)|(1&(i>>k));
    if(j<i){
      swap(&*(xr+i),&*(xr+j));
      swap(&*(xi+i),&*(xi+j));
    }
  }
  for(i=0;i<m;i++){
    n=pow(2.0,(float)i);
    w=PI/n;
    k=0;
    while(k<(numeroMuestras-1)){
      for(j=0;j<n;j++){
        arg=j*w;
        c=cos(arg);
        s=sin(arg);
        j1=k+j;
        tempr = *(xr+(j1+n))*c-*(xi+(j1+n))*s;
        tempi=*(xi+(j1+n))*c+*(xr+(j1+n))*s;
        *(xr+(j1+n))=*(xr+j1)-tempr;
        *(xi+(j1+n))=*(xi+j1)-tempi;
        *(xr+j1)=*(xr+j1)+tempr;
        *(xi+j1)=*(xi+j1)+tempi;
      }
      k+=2*n;
    }
  }
  for(i=0;i<numeroMuestras;i++){
    *(signalIFFT+(2*i))=*(xr+i);
    *(signalIFFT+((2*i)+1))=*(xi+i);
  }
  return signalIFFT;
}

void swap(float *a,float *b){
  float aux;
  aux=*a;
  *a=*b;
  *b=aux;
}

void initIFFT(char *inputRead, char *outputWrite) {
  FILE *fileRead,*fileWrite;
  short *signal,*canal1,*canal2,*muestras;
  datoCab *c = NULL;
  char *pie = NULL;
  int numeroMuestras = 0,i,numeroMuestrasCanales = 0,bytesPie = 0;
  float *signalIFFT,*xr,*xi;

  c=(datoCab*)malloc(sizeof(datoCab));
  fileRead=fopen(inputRead, "rb");
  if(fileRead==NULL){
    printf(BRED"Error al abrir archivo: %s\n"KNRM,inputRead);
    exit(EXIT_FAILURE);
  }
  c=readCabecera(c, fileRead);
  numeroMuestras=c->subChunk2Size/(c->bitsPerSample/8);
  numeroMuestrasCanales=(numeroMuestras/2);
  bytesPie=(c->chunkSize)-(c->subChunk2Size)-36;
  signal=(short*)malloc(sizeof(short)*numeroMuestras);
  canal1=(short*)malloc(sizeof(short)*numeroMuestrasCanales);
  canal2=(short*)malloc(sizeof(short)*numeroMuestrasCanales);
  xr=(float*)malloc(sizeof(float)*numeroMuestrasCanales);
  xi=(float*)malloc(sizeof(float)*numeroMuestrasCanales);
  pie=(char*)malloc(sizeof(char)*bytesPie);

  signal=sampleSignal(fileRead, numeroMuestras, signal);
  canal1=channel1(signal, canal1, numeroMuestrasCanales);
  canal2=channel2(signal, canal2, numeroMuestrasCanales);
  pie=readPie(fileRead, pie, bytesPie);
  fclose(fileRead);
  for(i=0;i<numeroMuestrasCanales;i++) {
    *(xr+i)=*(canal1+i);
    *(xi+i)=*(canal2+i);
  }
  signalIFFT=(float*)malloc(sizeof(float) * numeroMuestras);
  muestras=(short*)malloc(sizeof(short) * numeroMuestras);

  signalIFFT = ifft(xr, xi, numeroMuestrasCanales, signalIFFT);

  for(i=0;i<numeroMuestras;i++) {
    *(muestras+i)=*(signalIFFT+i);
  }

  fileWrite=fopen(outputWrite,"wb");
  if(fileWrite==NULL){
    printf(BRED"Error al abrir archivo: %s\n"KNRM,outputWrite);
    exit(EXIT_FAILURE);
  }
  fwrite(c,44,1,fileWrite);
  fwrite(muestras,sizeof(short)*c->subChunk2Size, 1,fileWrite);
  writePie(fileWrite,pie,bytesPie);

  fclose(fileWrite);
  free(c);
  free(signal);
  free(canal1);
  free(canal2);
  free(pie);
  free(signalIFFT);
  free(muestras);
  free(xr);
  free(xi);
}
