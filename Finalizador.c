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
#include <signal.h> 
#include "Pagina.h"
#include "Configuraciones.h"
#include "ProcesoGeneral.h"


#define tamanioPasoArchivo 100

//kill -9 1317

char *bitacora;
int *pID;
ProcesoGeneral *procesosEnMemoria;

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

	
	if(strcmp("", contenidoArchivo) == 0){
		return 0;
	}

	else{
		idMemoriaCompartida = atoi(contenidoArchivo); /*Se pasa de char* a int*/

		return idMemoriaCompartida;
	}



}

int vaciarTxtMemoriaCompartida(char *nombreArchivo){

	FILE *archivo;
	//char *contenidoArchivo;

	
	//contenidoArchivo = (char*)malloc(sizeof(char*));
	archivo = fopen(nombreArchivo, "w");

	if(archivo == NULL){

		printf("** No se pudo abrir el archivo correctamente **\n");

	}

	//sprintf(contenidoArchivo,"%i",idMemoriaCompartida);
	fprintf(archivo, "%s", "");

	fclose(archivo);

	return 0;


}

void liberarProductorProcesos(){

	int pId;
	pId = leerIdMemoriaCompartida("pid.txt");
	pID = shmat(pId, NULL, 0);
	kill(pID[0], SIGQUIT);

	printf("--- Se ha matado al proceso del id: %i ---\n", pID[0]);

	shmctl(pId, IPC_RMID, NULL);

	vaciarTxtMemoriaCompartida("pid.txt");

	printf("--- Se ha liberado la memoria compartida para el pId del productor de procesos ---\n");



}

void liberarMemoriaCompartida(){

	int idMemoriaCompartida;
	idMemoriaCompartida = leerIdMemoriaCompartida("idMemoriaCompartida.txt");
	shmctl(idMemoriaCompartida, IPC_RMID, NULL);

	vaciarTxtMemoriaCompartida("idMemoriaCompartida.txt");


	printf("--- Se ha liberado la memoria compartida ---\n");
	//shmdt(shm_ptr);

}

void liberarProcesosEnMemoria(){

	int idProcesosEnMemoria;
	idProcesosEnMemoria = leerIdMemoriaCompartida("procesosMemoria.txt");

	shmctl(idProcesosEnMemoria, IPC_RMID, NULL);

	vaciarTxtMemoriaCompartida("procesosMemoria.txt");


	printf("--- Se ha liberado la memoria compartida de los procesos en memoria ---\n");
	

}

void liberarProcesoPideMemoria(){

	int idProcesoPideMemoria;
	idProcesoPideMemoria = leerIdMemoriaCompartida("procesoPideMemoria.txt");

	shmctl(idProcesoPideMemoria, IPC_RMID, NULL);

	vaciarTxtMemoriaCompartida("procesoPideMemoria.txt");


	printf("--- Se ha liberado la memoria compartida del proceso que pide memoria ---\n");
	

}

void liberarProcesosBloqueados(){

	int idProcesosBloqueados;
	idProcesosBloqueados = leerIdMemoriaCompartida("procesosBloqueados.txt");

	shmctl(idProcesosBloqueados, IPC_RMID, NULL);

	vaciarTxtMemoriaCompartida("procesosBloqueados.txt");


	printf("--- Se ha liberado la memoria compartida de los procesos bloqueados ---\n");
	
	
}

void liberarProcesosMuertos(){

	int idProcesosMuertos;
	idProcesosMuertos = leerIdMemoriaCompartida("procesosMuertos.txt");

	vaciarTxtMemoriaCompartida("procesosMuertos.txt");

	shmctl(idProcesosMuertos, IPC_RMID, NULL);

	printf("--- Se ha liberado la memoria compartida de los procesos muertos ---\n");
	

}

void liberarProcesosTerminados(){

	int idProcesosTerminados;
	idProcesosTerminados = leerIdMemoriaCompartida("procesosTerminados.txt");

	shmctl(idProcesosTerminados, IPC_RMID, NULL);

	vaciarTxtMemoriaCompartida("procesosTerminados.txt");

	printf("--- Se ha liberado la memoria compartida de los procesos terminados ---\n");
	

}


void guardarBitacora(){

	int idBitacora;
	FILE *archivo;
	idBitacora = leerIdMemoriaCompartida("idMemoriaCompartidaBitacora.txt");
	bitacora = (char *)  shmat(idBitacora, NULL, 0);
	archivo = fopen("Bitacora.txt", "w");

	if(archivo == NULL){

		printf("** No se pudo abrir el archivo correctamente **\n");

	}
	
	fprintf(archivo, "%s", bitacora);

	

	//Guardar en un txt

	printf("--- Se ha guardado el contenido de la bitácora en un archivo txt ---\n");



}

void liberarBitacora(){

	int idBitacora;
	idBitacora = leerIdMemoriaCompartida("idMemoriaCompartidaBitacora.txt");

	shmctl(idBitacora, IPC_RMID, NULL);

	vaciarTxtMemoriaCompartida("idMemoriaCompartidaBitacora.txt");

	printf("--- Se ha liberado la memoria compartida de la bitácora ---\n");
	
}


int main(){

	printf("--- Bienvenido, este programa se encargará de finalizar todos los procesos, devolver la memoria compartida y guardar la bitácora ---\n");
	
	printf("-------------------------------------------------------------------------------\n");
	printf("               ...... \n");
	printf("            .:||||||||:.\n");
	printf("           |            | \n");
	printf("          (   o      o   )\n");
	printf("--@@@@----------:  :----------@@@@--\n");
	printf("Bienvenido, este programa se encargará:\n");
	printf("De finalizar todos los procesos\n");
	printf("Devolver la memoria compartida\n");
	printf("Guardar la bitácora en un txt\n");


	printf("-------------------------------------------------------------------------------\n");
	

	int idMemoriaCompartida;
	int idMemoriaCompartidaBitacora;
	int memoria;
	int pide;
	int bloqueado;
	int muerto;
	int terminado;
	int idPid;


	idMemoriaCompartida = leerIdMemoriaCompartida("idMemoriaCompartida.txt");
	idMemoriaCompartidaBitacora = leerIdMemoriaCompartida("idMemoriaCompartidaBitacora.txt");
	memoria = leerIdMemoriaCompartida("procesosMemoria.txt");
	pide = leerIdMemoriaCompartida("procesoPideMemoria.txt");
	bloqueado = leerIdMemoriaCompartida("procesosBloqueados.txt");
	muerto = leerIdMemoriaCompartida("procesosMuertos.txt");
	terminado = leerIdMemoriaCompartida("procesosTerminados.txt");
	idPid = leerIdMemoriaCompartida("pid.txt");


	if(idMemoriaCompartida == 0 && idMemoriaCompartidaBitacora == 0 && memoria == 0 && pide == 0 && bloqueado == 0 && muerto == 0
	 && terminado == 0 && idPid == 0){

	 	printf("--- No se puede acceder a memoria compartida, puesto que no se ha habilitado aún ---");
	 	return 1; 

	}

	else{

		liberarProductorProcesos();
		liberarMemoriaCompartida();
		liberarProcesosEnMemoria();
		liberarProcesoPideMemoria();
		liberarProcesosBloqueados();
		liberarProcesosMuertos();
		liberarProcesosTerminados();
		guardarBitacora();
		liberarBitacora();

		printf("--- ¡Se han liberado exitosamente todos los segmentos de memoria compartida! ---\n");


		return 1;
	}
}