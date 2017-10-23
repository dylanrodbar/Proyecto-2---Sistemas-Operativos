/*Programa encargado de liberar la memoria compartida, además de parar todos los procesos en ejecución y finalizar la bitácora*/



#include <sys/types.h> /*Manejo de memoria compartida*/
#include <sys/ipc.h>   /*Manejo de memoria compartida*/
#include <sys/shm.h>   /*Manejo de memoria compartida*/
#include <stdio.h> 
#include <string.h>
#include <pthread.h>   /*Para el manejo de  hilos*/
#include <time.h>
#include <stdlib.h>    /*Para el uso de rand()*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "Pagina.h"
#include "Configuraciones.h"
#include "ProcesoGeneral.h"


#define tamanioPasoArchivo 100

char *bitacora;

/*Función encargada de leer el txt que contiene el Id de la memoria compartida que se desea usar*/
int leerIdMemoriaCompartida(char *nombreArchivo){

	char *contenidoArchivo;
	size_t nread; 
	FILE *archivo;
	int idMemoriaCompartida;
	int tamanioTotalPasoArchivo;

	contenidoArchivo= (char*)malloc(sizeof(char*));
	archivo = fopen(nombreArchivo, "r");

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

void liberarMemoriaCompartida(){

	int idMemoriaCompartida;
	idMemoriaCompartida = leerIdMemoriaCompartida("idMemoriaCompartida.txt");
	shmctl(idMemoriaCompartida, IPC_RMID, NULL);

	printf("--- Se ha liberado la memoria compartida ---\n");
	//shmdt(shm_ptr);

}

void liberarProcesosEnMemoria(){

	int idProcesosEnMemoria;
	idProcesosEnMemoria = leerIdMemoriaCompartida("procesosMemoria.txt");

	shmctl(idProcesosEnMemoria, IPC_RMID, NULL);

	printf("--- Se ha liberado la memoria compartida de los procesos en memoria ---\n");
	

}

void liberarProcesoPideMemoria(){

	int idProcesoPideMemoria;
	idProcesoPideMemoria = leerIdMemoriaCompartida("procesoPideMemoria.txt");

	shmctl(idProcesoPideMemoria, IPC_RMID, NULL);

	printf("--- Se ha liberado la memoria compartida del proceso que pide memoria ---\n");
	

}

void liberarProcesosBloqueados(){

	int idProcesosBloqueados;
	idProcesosBloqueados = leerIdMemoriaCompartida("procesosBloqueados.txt");

	shmctl(idProcesosBloqueados, IPC_RMID, NULL);

	printf("--- Se ha liberado la memoria compartida de los procesos bloqueados ---\n");
	
	
}

void liberarProcesosMuertos(){

	int idProcesosMuertos;
	idProcesosMuertos = leerIdMemoriaCompartida("procesosMuertos.txt");

	shmctl(idProcesosMuertos, IPC_RMID, NULL);

	printf("--- Se ha liberado la memoria compartida de los procesos muertos ---\n");
	

}

void liberarProcesosTerminados(){

	int idProcesosTerminados;
	idProcesosTerminados = leerIdMemoriaCompartida("procesosTerminados.txt");

	shmctl(idProcesosTerminados, IPC_RMID, NULL);

	printf("--- Se ha liberado la memoria compartida de los procesos terminados ---\n");
	

}

void guardarBitacora(){

	int idBitacora;
	idBitacora = leerIdMemoriaCompartida("idMemoriaCompartidaBitacora.txt");
	bitacora = (char *)  shmat(idBitacora, NULL, 0);

	//Guardar en un txt

	printf("--- Se ha guardado el contenido de la bitácora en un archivo txt ---\n");



}

void liberarBitacora(){

	int idBitacora;
	idBitacora = leerIdMemoriaCompartida("idMemoriaCompartidaBitacora.txt");

	shmctl(idBitacora, IPC_RMID, NULL);

	printf("--- Se ha liberado la memoria compartida de la bitácora ---\n");
	
}


int main(){

	printf("--- Bienvenido, este programa se encargará de finalizar todos los procesos, devolver la memoria compartida y guardar la bitácora ---\n");
	
	liberarMemoriaCompartida();
	liberarProcesosEnMemoria();
	liberarProcesoPideMemoria();
	liberarProcesosBloqueados();
	liberarProcesosMuertos();
	liberarProcesosTerminados();
	guardarBitacora();
	liberarBitacora();


	return 1;
}