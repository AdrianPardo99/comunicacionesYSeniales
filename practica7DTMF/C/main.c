/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 02-05-2020 */
#include "cabeceraWAV.h"

int main(int argc, char **argv) {
  if(argc<2){
    printf("Error\nPlease Usage: %s <input-wav>",*argv);
    exit(EXIT_FAILURE);
  }
  initDMTF(*(argv+1));
}
