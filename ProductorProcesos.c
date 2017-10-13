/* Este programa estará encargado de pedir la memoria compartida
   incluyendo las páginas que requerirá para los procesos */

#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <stdio.h> 

#define tamanioPasoArchivo 100 /*Tamaño máximo para pasar el contenido del txt a una variable*/



/*Función encargada de leer el txt que contiene el Id de la memoria compartida que se desea usar*/
int leerIdMemoriaCompartida(){

	char *contenidoArchivo;
	size_t nread; 
	FILE *archivo;
	int idMemoriaCompartida;
	int tamanioTotalPasoArchivo;

	contenidoArchivo= (char*)malloc(sizeof(char*));
	archivo = fopen("idMemoriaCompartida.txt", "r");

	if (archivo == NULL) {
   
		printf("** No se pudo abrir el archivo correctamente **\n");

	}

	tamanioTotalPasoArchivo = 0;

	/*Se para el contenido del txt a una variable*/
	while ((nread = fread(contenidoArchivo, 1, tamanioPasoArchivo, archivo)) > 0) {
    	
    	tamanioTotalPasoArchivo = tamanioTotalPasoArchivo + tamanioPasoArchivo;
    	//printf("Cargados: %i bytes\n", tamanioTotalPasoArchivo);
	
	}

	printf("*** Id de memoria compartida cargado con éxito ***\n");


	idMemoriaCompartida = atoi(contenidoArchivo); /*Se pasa de char* a int*/

	return idMemoriaCompartida;



}

/* Función encargada de solicitar memoria compartida al sistema operativo */
int leerMemoria(int idMemoriaCompartida){

	key_t key; /*Clave que se pasa al shmget*/
	int banderaMemoriaCompartida; /*Necesaria para saber bajo qué "modalidad" se crea la memoria compartida*/
	int tamanioMemoriaCompartida;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	char *memoriaCompartida; /*Con esta variable se puede acceder al contenido de la memoria compartida*/

	key = 5678;
	
	
	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartida < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida **\n");
		return -1;

	}

	
	memoriaCompartida = shmat(idMemoriaCompartida, NULL, 0); /*Se accede a la memoria compartida*/


	/*Se valida si se obtuvo acceso a la memoria compartida correctamente*/
	if(memoriaCompartida == (char *) -1){

		printf("Error");
		return -1;

	}
	
	printf("*** Contenido de la memoria: %s ***\n", memoriaCompartida);

	
	
	return 1;



}


/* Desde esta función se llamará a las funciones requeridas para cumplir con la funcionalidad de este programa */
void main(){
	
	int idMemoriaCompartida;
	idMemoriaCompartida = leerIdMemoriaCompartida();
	leerMemoria(idMemoriaCompartida);

}