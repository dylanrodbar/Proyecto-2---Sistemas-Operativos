/* Este programa estará encargado de pedir la memoria compartida
   incluyendo las páginas que requerirá para los procesos */

#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <stdio.h>
#include <signal.h> /*Necesaria para matar un proceso*/
#include <unistd.h> /*Necesaria para la función sleep*/


/*Función encargada de guardar en un txt el Id de la memoria compartida que se obtiene*/
int guardarIdMemoriaCompartida(int idMemoriaCompartida){
	
	FILE *archivo;
	char *contenidoArchivo;

	
	contenidoArchivo = (char*)malloc(sizeof(char*));
	archivo = fopen("idMemoriaCompartida.txt", "w");

	if(archivo == NULL){

		printf("** No se pudo abrir el archivo correctamente **\n");

	}

	sprintf(contenidoArchivo,"%i",idMemoriaCompartida);
	fprintf(archivo, "%s", contenidoArchivo);

	fclose(archivo);

	return 0;

}



/* Función encargada de solicitar memoria compartida al sistema operativo */
int solicitarMemoria(){
	
	key_t key; /*Clave que se pasa al shmget*/
	int banderaMemoriaCompartida; /*Necesaria para saber bajo qué "modalidad" se crea la memoria compartida*/
	int idMemoriaCompartida;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartida;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	char *memoriaCompartida; /*Con esta variable se puede acceder al contenido de la memoria compartida*/
	
	key = 6789;
	tamanioMemoriaCompartida = 32;
	banderaMemoriaCompartida = IPC_CREAT;
	idMemoriaCompartida = shmget(key, tamanioMemoriaCompartida, banderaMemoriaCompartida | 0666);


	
	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartida < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida **\n");
		return -1;

	}

	printf("*** Id generado para la memoria compartida: %i ***\n", idMemoriaCompartida);

	
	memoriaCompartida = shmat(idMemoriaCompartida, NULL, 0); /*Tener acceso a la memoria compartida*/


	/*Se valida si se obtuvo acceso a la memoria compartida correctamente*/
	if(memoriaCompartida == (char *) -1){

		printf("Error, no se obtuvo un acceso correcto a la memoria compartida");
		return -1;

	}

	strncpy(memoriaCompartida, "nuevocontenido", tamanioMemoriaCompartida);

	guardarIdMemoriaCompartida(idMemoriaCompartida);

	return idMemoriaCompartida;

}



/* Desde esta función se llamará a las funciones requeridas para cumplir con la funcionalidad de este programa */
void main(){

	int idMemoriaCompartida;
	printf("****************************************************************************\n");
	printf("Bienvenido(a), a continuación se le solicitará al sistema operativo memoria compartida, se le dará a conocer un Id para el mismo\n");
	printf("****************************************************************************\n");
	
	sleep(1);
	printf(".\n");
	sleep(1);
	printf("..\n");
	sleep(1);
	printf("...\n");
	sleep(1);


	idMemoriaCompartida = solicitarMemoria();
	if(idMemoriaCompartida > -1){
		printf("****************************************************************************\n");
		printf("El proceso se ha realizado correctamente, el Id para la memoria compartida es %i, también puede consultarlo en el archivo 'idMemoriaCompartida.txt' ", idMemoriaCompartida);
		printf("****************************************************************************\n");
	
	}
}