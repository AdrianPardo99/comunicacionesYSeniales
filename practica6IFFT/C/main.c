/* Developed by:
 * Gonzalez Pardo Adrian
 * 3CM7 20-02
 * Last file update: 30-04-2020 */

#include "cabeceraWAV.h"

 int main(int argc, char **argv) {
   if(argc<3){
     printf(BRED"Error"BYEL"\nPlease Usage: %s <input-wav> <output-wav>"KNRM,*argv);
     exit(EXIT_FAILURE);
   }
   initIFFT(*(argv+1), *(argv+2));
 }
