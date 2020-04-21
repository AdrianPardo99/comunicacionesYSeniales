/* Developed by:
* Gonzalez Pardo Adrian
* 3CM7 20-02
* Last file update: 21-04-2020 */


/* La practica consta de abrir un archivo wav y a traves de este programa
* reducir el volumen de la senial de entrada a un 50% del valor que tiene
* en el archivo y guardar el archivo con otro nombre, ambos archivos son
* manejados como argumentos... */
#include "cabeceraWAV.h"

int main(int argc, char *argv[]) {
  if(argc<3){
    printf(BRED"Error"BYEL"\nPlease Usage: %s <input-wav> <output-wav>",*argv);
    exit(EXIT_FAILURE);
  }
  operaCabeceraVolumen(*(argv+1),*(argv+2));
  return 0;
}
