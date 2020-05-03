/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 02-05-2020 */
#include "cabeceraWAV.h"

datoCab *readCabecera(datoCab* c, FILE* fileRead) {
  fread(c,44,1,fileRead);
  return c;
}

short *sampleSignal(FILE* fileRead,int numeroMuestras,short* signal) {
  fread(signal,numeroMuestras*sizeof(short),1,fileRead);
  return signal;
}

char *readPie(FILE* fileRead, char* pie, int bytesPie) {
  fread(pie,sizeof(char)*bytesPie,1,fileRead);
  return pie;
}

void writePie(FILE* fileWrite, char *pie, int bytesPie) {
  fwrite(pie,sizeof(char)*bytesPie,1,fileWrite);
}

short *channel1F(short *signal,short *canal1,int numeroMuestras) {
  for(int i=0;i<(numeroMuestras);i++)
    *(canal1+i)=*(signal+(2*i));
  return canal1;
}

short *channel2F(short *signal, short *canal2, int numeroMuestras) {
  for(int i=0;i<(numeroMuestras);i++)
    *(canal2+i) = *(signal+((2 * i)+1));
  return canal2;
}

double *multiplica(short *signalM,short *signalm,int NM,int Nm,double *s){
  int i=0;
  for(i=0;i<Nm;i++){
    *(s+i)=*(signalM+i)**(signalm+i);
    *(s+i)/=32767;
    if(*(s+i)>32767){
      *(s+i)=32767;
    }
  }
  for(i=Nm;i<NM;i++){
    *(s+i)=0;
  }
  return s;
}

short *multMonoMono(short *signalM,short *signalm,int NM,int Nm,double *signalMul,short *sample){
  int i;
  for(i=0;i<Nm;i++){
    *(signalMul+i)=*(signalM+i)* *(signalm+i);
    *(signalMul+i)/=32767;
    if(*(signalMul+i)>32767){
      *(signalMul+i)=32767;
    }
    *(sample+i)=*(signalMul+i);
  }
  for(i=Nm;i<NM;i++){
    *(sample+i)=0;
  }
  return sample;
}

short *multStereoStereo(short *channel1R, short *channel1L, short *channel2R,
    short *channel2L,int NM,int Nm,short *sample){
    int i;
    double *r1=(double*)malloc(sizeof(double)*NM),
           *r2=(double*)malloc(sizeof(double)*NM),
           *r3=(double*)malloc(sizeof(double)*NM),
           *r4=(double*)malloc(sizeof(double)*NM),
           *c1=(double*)malloc(sizeof(double)*NM),
           *c2=(double*)malloc(sizeof(double)*NM);
    r1=multiplica(channel1R,channel2R,NM,Nm,r1);
    r2=multiplica(channel1L,channel2L,NM,Nm,r2);
    r3=multiplica(channel1R,channel2L,NM,Nm,r3);
    r4=multiplica(channel1L,channel2R,NM,Nm,r4);
    for(i=0;i<NM;i++){
      *(c1+i)=(*(r1+i)-*(r2+i))/2;
      *(c2+i)=(*(r3+i)+*(r4+i))/2;
    }
    for(i=0;i<NM;i++){
      *(sample+(i*2))=*(c1+i);
      *(sample+((i*2)+1))=*(c2+i);
    }
    free(r1);
    free(r2);
    free(r3);
    free(r4);
    free(c1);
    free(c2);
    return sample;
}

void initMultiplicar(char *fileRead,char *fileRead1,char *fileWrite){
  FILE *read=NULL,*read1=NULL,*write=NULL;
  datoCab *c1=NULL,*c2=NULL;
  int N1=0,N2=0,t1=0,t2=0,nc1=0,nc2=0,bP=0,type=0;
  char *pie;
  double *signalMul;
  short *signal1,*signal2,*sample,*channel1,*channel2,*channel3,*channel4;
  read=fopen(fileRead,"rb");
  read1=fopen(fileRead1,"rb");
  write=fopen(fileWrite,"wb");
  if(read==NULL){
    printf("Error al abrir archivo: %s\n",fileRead);
    exit(EXIT_FAILURE);
  }
  if(read1==NULL){
    printf("Error al abrir archivo: %s\n",fileRead1);
    exit(EXIT_FAILURE);
  }
  if(write==NULL){
    printf("Error al abrir archivo: %s\n",fileWrite);
    exit(EXIT_FAILURE);
  }
  c1=(datoCab*)malloc(sizeof(datoCab));
  c2=(datoCab*)malloc(sizeof(datoCab));
  c1=readCabecera(c1,read);
  c2=readCabecera(c2,read1);
  t1=c1->chunkSize;
  t2=c2->chunkSize;
  N1=c1->subChunk2Size/(c1->bitsPerSample/8);
  N2=c2->subChunk2Size/(c2->bitsPerSample/8);
  nc1=c1->numChannels;
  nc2=c2->numChannels;
  bP=(c1->chunkSize)-(c1->subChunk2Size)-36;
  pie=(char*)malloc(sizeof(char)*bP);
  signal1=(short*)malloc(sizeof(short)*N1);
  signal2=(short*)malloc(sizeof(short)*N2);

  if(nc1==1){
    signal1=sampleSignal(read,N1,signal1);
  }else if(nc1==2){
    signal1=sampleSignal(read,N1,signal1);
    channel1=(short*)malloc(sizeof(short)*(N1/2));
    channel2=(short*)malloc(sizeof(short)*(N1/2));
    channel1=channel1F(signal1,channel1,(N1/2));
    channel2=channel2F(signal1,channel2,(N1/2));
  }
  if(nc2==1){
    signal2=sampleSignal(read1,N2,signal2);
  }else if(nc2==2){
    signal2=sampleSignal(read1,N2,signal2);
    channel3=(short*)malloc(sizeof(short)*(N2/2));
    channel4=(short*)malloc(sizeof(short)*(N2/2));
    channel3=channel1F(signal2,channel3,(N2/2));
    channel4=channel2F(signal2,channel4,(N2/2));
  }
  if(nc1==1&&nc2==1){
    type=1;
  }else if(nc1==2&&nc2==2){
    type=2;
  }
  if(type==1){
    if(t1>=t2){
      fwrite(c1,44,1,write);
      sample=(short*)malloc(sizeof(short)*N1);
      signalMul=(double*)malloc(sizeof(double)*N1);
      sample=multMonoMono(signal1,signal2,N1,N2,signalMul,sample);
      fwrite(sample,sizeof(short)*N1,1,write);
      fwrite(pie,sizeof(char)*bP,1,write);
    }else{
      fwrite(c2,44,1,write);
      sample=(short*)malloc(sizeof(short)*N2);
      signalMul=(double*)malloc(sizeof(double)*N2);
      sample=multMonoMono(signal2,signal1,N2,N1,signalMul,sample);
      fwrite(sample,sizeof(short)*N2,1,write);
      fwrite(pie,sizeof(char)*bP,1,write);
    }
  }else if(type==2){
    if(t1>=t2){
      fwrite(c1,44,1,write);
      sample=(short*)malloc(sizeof(short)*N1);
      signalMul=(double*)malloc(sizeof(double)*N1);
      sample=multStereoStereo(channel1,channel2,channel3,channel4,(N1/2),(N2/2),sample);
      fwrite(sample,sizeof(short)*N1,1,write);
      fwrite(pie,sizeof(char)*bP,1,write);
    }else{
      fwrite(c2,44,1,write);
      sample=(short*)malloc(sizeof(short)*N2);
      signalMul=(double*)malloc(sizeof(double)*N2);
      sample=multStereoStereo(channel3,channel4,channel1,channel2,(N2/2),(N1/2),sample);
      fwrite(sample,sizeof(short)*N2,1,write);
      fwrite(pie,sizeof(char)*bP,1,write);
    }
    free(channel1);
    free(channel2);
    free(channel3);
    free(channel4);
  }


  fclose(read);
  fclose(read1);
  fclose(write);
  free(c1);
  free(c2);
  free(pie);
  free(signal1);
  free(signal2);
  free(sample);
  free(signalMul);
}
