/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 28-04-2020 */
#include "cabeceraWAV.h"

int main(int argc, char **argv) {
  if(argc<3){
    printf("Error\nPlease Usage: %s <input-wav> <output-wav>",*argv);
    exit(EXIT_FAILURE);
  }
  initFFT(*(argv+1), *(argv+2));
}
