/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 24-04-2020 */
#include "cabeceraWAV.h"

datoCab *readCabecera(datoCab *c,FILE *read){
  fread(c,44,1,read);
  return c;
}


char *readPie(FILE *read,int t,char *dst){
  fread(dst,sizeof(char)*t,1,read);
  return dst;
}

void copiaPie(FILE *write,int t,char *src){
  fwrite(src,sizeof(char)*t,1,write);
}

int isMono(int signal){
  return (signal==1);
}

void monoToStereo(datoCab *cab,FILE *write){
  cab->chunkSize=(cab->chunkSize + cab->subChunk2Size);
  cab->numChannels=2;
  cab->byteRate*=2;
  cab->blockAlign*=2;
  cab->subChunk2Size*=2;
  fwrite(cab,44,1,write);
}

short *showSignal(FILE *read,int t,short *arr){
  fread(arr,sizeof(short)*t,1,read);
  return arr;
}

double *dft(int t,short *signal,double *signalDFT){
  int k,n;
  for(k=0;k<t;k++){
    *(signalDFT+(2*k))=0;
    *(signalDFT+((2*k)+1))=0;
    for(n=0;n<t;n++){
      *(signalDFT+(2*k))+=*(signal+n)*cos((2*PI*k*n)/t);
      *(signalDFT+((2*k)+1))+=*(signal+n)*sin((2*PI*k*n)/t);
    }
    *(signalDFT+(2*k))/=t;
    *(signalDFT+((2*k)+1))/=t;
  }
  return signalDFT;
}

void initDFT(char *src,char *dst){
  FILE *in,*out;
  int N,bP,i;
  datoCab *cab;
  short *signal,*muestra;
  double *signalFT;
  char *datPie;

  in=fopen(src,"rb");
  out=fopen(dst,"wb");
  if(in==NULL){
    printf("Error al abrir archivo: %s\n",src);
    exit(EXIT_FAILURE);
  }
  if(out==NULL){
    printf("Error al abrir archivo: %s\n",dst);
    exit(EXIT_FAILURE);
  }
  cab=(datoCab*)malloc(sizeof(datoCab));
  cab=readCabecera(cab,in);
  if(!isMono(cab->audioFormat)){
    printf("Error formato equivoco\n");
    free(cab);
    exit(EXIT_FAILURE);
  }
  N=cab->subChunk2Size/(cab->byteRate/8);
  signal=(short*)malloc(sizeof(short)*N);
  signal=showSignal(in,N,signal);
  bP=(cab->chunkSize-cab->subChunk2Size-36);
  datPie=(char*)malloc(sizeof(char)*bP);
  datPie=readPie(in,bP,datPie);
  monoToStereo(cab,out);
  signalFT=(double*)malloc(sizeof(double)*(N*2));
  muestra=(short*)malloc(sizeof(short)*(N*2));
  signalFT=dft(N,signal,signalFT);
  for(i=0;i<N*2;i++){
    *(muestra+i)=*(signalFT+i);
  }
  fwrite(muestra,cab->subChunk2Size,1,out);
  copiaPie(out,bP,datPie);
  fclose(in);
  fclose(out);
  free(signal);
  free(signalFT);
  free(muestra);
  free(datPie);

}
