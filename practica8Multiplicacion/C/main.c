/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 02-05-2020 */
#include "cabeceraWAV.h"

int main(int argc, char **argv) {
  if(argc<4){
    printf(BRED"Error\n"BYEL"Please Usage: %s <input-wav> <input-wav2> <output-wav>"KNRM,*argv);
    exit(EXIT_FAILURE);
  }
  initMultiplicar(*(argv+1),*(argv+2),*(argv+3));
}
