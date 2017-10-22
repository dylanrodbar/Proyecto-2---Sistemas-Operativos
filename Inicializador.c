/* Este programa estará encargado de pedir la memoria compartida
   incluyendo las páginas que requerirá para los procesos */

#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <stdio.h>
#include <signal.h> /*Necesaria para matar un proceso*/
#include <unistd.h> /*Necesaria para la función sleep*/
#include "Pagina.h"
#include "Configuraciones.h"

Pagina *paginas;



/*---Función encargada de guardar en un txt el Id de la memoria compartida que se obtiene---*/
int guardarIdMemoriaCompartida(int idMemoriaCompartida, char* nombreArchivo){
	
	FILE *archivo;
	char *contenidoArchivo;

	
	contenidoArchivo = (char*)malloc(sizeof(char*));
	archivo = fopen(nombreArchivo, "w");

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
	
	/*----------------------------------------------------------------------------------------------------*/
	key_t key; /*Clave que se pasa al shmget*/
	int banderaMemoriaCompartida; /*Necesaria para saber bajo qué "modalidad" se crea la memoria compartida*/
	int idMemoriaCompartida;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartida;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	char *memoriaCompartida; /*Con esta variable se puede acceder al contenido de la memoria compartida*/
	/*----------------------------------------------------------------------------------------------------*/
	
	/*----------------------------------------------------------------------------------------------------*/
	key_t keyBitacora; /*Clave que se pasa al shmget*/
	int idMemoriaCompartidaBitacora;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartidaBitacora;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	/*----------------------------------------------------------------------------------------------------*/
	
	/*----------------------------------------------------------------------------------------------------*/
	key_t keyEspia; /*Clave que se pasa al shmget*/
	int idMemoriaCompartidaEspia;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartidaEspia;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	/*----------------------------------------------------------------------------------------------------*/
	
	/*----------------------------------------------------------------------------------------------------*/
	key = 5559;
	tamanioMemoriaCompartida = tamanio * sizeof(struct Pagina);
	banderaMemoriaCompartida = IPC_CREAT;
	idMemoriaCompartida = shmget(key, tamanioMemoriaCompartida, banderaMemoriaCompartida | 0666);
	/*----------------------------------------------------------------------------------------------------*/
	
	/*----------------------------------------------------------------------------------------------------*/
	keyBitacora = 5555;
	tamanioMemoriaCompartidaBitacora = tamanioBitacora;
	idMemoriaCompartidaBitacora = shmget(keyBitacora, tamanioMemoriaCompartidaBitacora, banderaMemoriaCompartida | 0666);
	/*----------------------------------------------------------------------------------------------------*/
	

	/*----------------------------------------------------------------------------------------------------*/
	keyEspia = 5556;
	tamanioMemoriaCompartidaEspia = tamanioEspia;
	idMemoriaCompartidaEspia = shmget(keyEspia, tamanioMemoriaCompartidaEspia, banderaMemoriaCompartida | 0666);
	/*----------------------------------------------------------------------------------------------------*/
	

	
	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartida < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida **\n");
		return -1;

	}

	paginas = (Pagina *) shmat(idMemoriaCompartida, NULL, 0); /*Cast*/
	
	/*Valida el correcto acceso a la memoria compartida*/
	if(paginas == (Pagina *) -1){

		printf("Hubo un error mientras se intentaba accesar a la memoria compartida");
		return -1;
	
	}

	for(int i = 0; i<tamanio; i++){

		paginas[i].numeroPagina = i;
		paginas[i].disponible = 1;
		paginas[i].procesoOcupado = -1; /*Para indicar que no tiene ningún proceso encima*/
		paginas[i].numeroSegmento = -1; /*Para indicar que en el momento, no está siendo utilizado por ningún segmentos*/

	}
	
	guardarIdMemoriaCompartida(idMemoriaCompartida, "idMemoriaCompartida.txt");
	guardarIdMemoriaCompartida(idMemoriaCompartidaBitacora, "idMemoriaCompartidaBitacora.txt");
	guardarIdMemoriaCompartida(idMemoriaCompartidaEspia, "idMemoriaCompartidaEspia.txt");


	return idMemoriaCompartida;

}



/* Desde esta función se llamará a las funciones requeridas para cumplir con la funcionalidad de este programa */
void main(){

	int idMemoriaCompartida;
	printf("-------------------------------------------------------------------------------\n");
	printf("Bienvenido(a), a continuación se le solicitará al sistema operativo memoria compartida, se le dará a conocer un Id para el mismo\n");
	printf("-------------------------------------------------------------------------------\n");
	
	printf(".\n");
	printf("..\n");
	printf("...\n");
	
	idMemoriaCompartida = solicitarMemoria();
	if(idMemoriaCompartida > -1){
		printf("-------------------------------------------------------------------------------\n");
		printf("El proceso se ha realizado correctamente, el Id para la memoria compartida es %i, también puede consultarlo en el archivo 'idMemoriaCompartida.txt'\n", idMemoriaCompartida);
		printf("-------------------------------------------------------------------------------\n");
	
	}
}