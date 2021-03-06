/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 21-04-2020 */
#include "cabeceraWAV.h"

void operaCabeceraVolumen(char *input,char *output){
  FILE *in,*out;
  int i,N;
  short int m;
  char *mm;
  datoCab cab;
  in=fopen(input,"rb");
  if(in==NULL){
    printf(BRED"Error al abrir el archivo %s\n"KNRM,input);
    exit(EXIT_FAILURE);
  }
  out=fopen(output,"wb");
  if(out==NULL){
    printf(BRED"Error al abrir el archivo %s\n"KNRM,output);
    exit(EXIT_FAILURE);
  }
  fread(&cab,44,1,in);
  fwrite(&cab,44,1,out);
  muestraCabeceraArchivo(&cab);
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

void muestraCabeceraArchivo(datoCab *cab){
  printf(BCYN"\t(1-4 bytes) Chunk ID: %s\n"
    BYEL"\t(5-8 bytes) Chunk Size: %u\n"
    BBLU"\t(9-12 bytes) Format: %s\n"
    BGRN"\t(13-16 bytes) Sub Chunk 1 ID: %s\n"
    KGRN"\t(17-20 bytes) Sub Chunk 1 Size: %u\n"
    KBLU"\t(21-22 bytes) Audio Format: %u,%s\n"
    KYEL"\t(23-24 bytes) Number of Channels: %u, Type: %s\n"
    BMAG"\t(25-28 bytes) Sample Rate: %u\n"
    BYEL"\t(29-32 bytes) Byte Rate: %u\n"
    BBLU"\t(33-34 bytes) Block Align: %u\n"
    BGRN"\t(35-36 bytes) Bits per sample: %u\n"
    KGRN"\t(37-40 bytes) Sub Chunk 2 ID: %s\n"
    KBLU"\t(41-44 bytes) Sub Chunk 2 Size: %u\n\n"KNRM
    ,cab->chunkID,cab->chunkSize,cab->format,cab->subChunk1Id,
    cab->subChunk1Size,cab->audioFormat,(cab->audioFormat==1)?("PCM"):(""),
    cab->numChannels,(cab->numChannels==1)?("Mono"):("Estereo"),
    cab->sampleRate,cab->byteRate,cab->blockAlign,
    cab->bitsPerSample,cab->subChunk2Id,cab->subChunk2Size);
}
