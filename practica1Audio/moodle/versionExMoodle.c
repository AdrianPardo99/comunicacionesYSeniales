#include <stdio.h>
#include <stdlib.h>


typedef struct datos{
  char chunkID[4];
  int chunkSize;
  char format[4];
  char subChunk1Id[4];
  int subChunk1Size;
  short int audioFormat;
  short int numChannels;
  int sampleRate;
  int byteRate;
  short int blockAlign;
  short int bitsPerSample;
  char subChunk2Id[4];
  int subChunk2Size;
}datoCab;

void operaCabeceraVolumen(char*,char*);

int main(int argc, char *argv[]) {
  if(argc!=3){
    printf("Error\nPlease Usage: %s <input-wav> <output-wav>",*argv);
    exit(EXIT_FAILURE);
  }
  operaCabeceraVolumen(*(argv+1),*(argv+2));
  return 0;
}

void operaCabeceraVolumen(char *input,char *output){
  FILE *in,*out;
  int i,N;
  short int m;
  char *mm;
  datoCab cab;
  in=fopen(input,"rb");
  if(in==NULL){
    printf("Error al abrir el archivo %s\n",input);
    exit(EXIT_FAILURE);
  }
  out=fopen(output,"wb");
  if(out==NULL){
    printf("Error al abrir el archivo %s\n",output);
    exit(EXIT_FAILURE);
  }
  fread(&cab,44,1,in);
  fwrite(&cab,44,1,out);
  N=(cab.subChunk2Size/cab.blockAlign);
  for(i=0;i<N;i++){
    fread(&m,sizeof(short int),1,in);
    m*=0.5;
    fwrite(&m,sizeof(short int),1,out);
  }
  N=cab.chunkSize-cab.subChunk2Size-36;
  mm=(char*)malloc(sizeof(char)*N);
  fread(mm,N,1,in);
  fwrite(mm,N,1,out);
  fclose(in);
  fclose(out);
  free(mm);
}
