#include "cabeceraWAV.h"

int main(int argc, char *argv[]) {
  if(argc<3){
    printf(BRED"Error"BYEL"\nPlease Usage: %s <input-wav> <output-wav>",*argv);
    exit(EXIT_FAILURE);
  }
  initIDFT(*(argv+1),*(argv+2));
  return 0;
}
