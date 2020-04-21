/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 21-04-2020 */
#include "cabeceraWAV.h"

float *getFiltro(int n){
  int i;
  float *f=(float*)malloc(sizeof(float)*n);
  for(i=0;i<n;i++){
    *(f+i)=exp(-i*constante);
  }
  return f;
}

void copiaCabecera(datoCab *cab,FILE *in,FILE *out){
    fread(cab,44,1,in);
    fwrite(cab,44,1,out);
}

void copiaPie(datoCab *cab,FILE *in,FILE *out){
  int N=cab->chunkSize-cab->subChunk2Size-36;
  char *bytes=(char*)malloc(sizeof(char)*N);
  fread(bytes,sizeof(char)*N,1,in);
  fwrite(bytes,sizeof(char)*N,1,out);
  free(bytes);
}

float absoluto(float f){
  return (f<0)?(-f):(f);
}

void operaConvolucion(char *fileIn,char *fileOut){
  float *filtro,*conv,*entrada,*respuesta,outMax=0,inMax=0,escala=0;
  FILE *in,*out;
  short muestra;
  int numeroMuestras,i,j;
  datoCab *cab=(datoCab*)malloc(sizeof(datoCab));
  filtro=getFiltro(cantidadMuestras);
  in=fopen(fileIn,"rb");
  out=fopen(fileOut,"wb");
  if(in==NULL){
    printf("Error al abrir el archivo %s\n",fileIn);
    exit(EXIT_FAILURE);
  }
  if(out==NULL){
    printf("Error al abrir el archivo %s\n",fileOut);
    exit(EXIT_FAILURE);
  }
  copiaCabecera(cab,in,out);
  numeroMuestras=(cab->subChunk2Size/(cab->bitsPerSample/8));
  entrada=(float*)malloc(sizeof(float)*numeroMuestras);
  respuesta=(float*)malloc(sizeof(float)*numeroMuestras);
  conv=(float*)malloc(sizeof(float)*cantidadMuestras);

  for(i=0;i<numeroMuestras;i++){
    fread(&muestra,sizeof(short),1,in);
    *(entrada+i)=muestra;
  }
  inMax=absoluto(*(entrada));
  for(i=1;i<numeroMuestras;i++){
    if(inMax<absoluto(*(entrada+i))){
      inMax=absoluto(*(entrada+i));
    }
  }
  if(inMax>32767){
    inMax=32767;
  }
  for(i=0;i<numeroMuestras;i++){
    for(j=cantidadMuestras-1;j>=1;j--)*(conv+j)=*(conv+(j-1));
    conv[0]=entrada[i];
    for(j=0;j<cantidadMuestras;j++)*(respuesta+i)+=*(filtro+j)* *(conv+j);
  }
  outMax=absoluto(*(respuesta));
  for(i=1;i<numeroMuestras;i++){
    if(outMax<absoluto(*(respuesta+i))){
      outMax=absoluto(*(respuesta+i));
    }
  }
  escala=inMax/outMax;
  for(i=0;i<numeroMuestras;i++){
    muestra=*(respuesta+i)*escala;
    fwrite(&muestra,sizeof(short),1,out);
  }
  copiaPie(cab,in,out);
  fclose(in);
  fclose(out);
  free(cab);
  free(entrada);
  free(respuesta);
  free(conv);
  free(filtro);
}
