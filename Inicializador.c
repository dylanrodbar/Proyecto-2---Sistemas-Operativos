/* Este programa estará encargado de pedir la memoria compartida
   incluyendo las páginas que requerirá para los procesos */

#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <signal.h> /*Necesaria para matar un proceso*/
#include <unistd.h> /*Necesaria para la función sleep*/
#include "Pagina.h"
#include "Configuraciones.h"
#include "ProcesoGeneral.h"

Pagina *paginas;
ProcesoGeneral *procesosEnMemoria;
ProcesoGeneral *procesoPideMemoria;
ProcesoGeneral *procesosBloqueados;
ProcesoGeneral *procesosMuertos;
ProcesoGeneral *procesosTerminados;
char *bitacora;
int pId;



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
	key_t keyPID; /*Clave que se pasa al shmget*/
	int idMemoriaCompartidaPID;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartidaPID;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	/*----------------------------------------------------------------------------------------------------*/
	

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
	key_t keyEspiaProcesosMemoria; /*Clave que se pasa al shmget*/
	int idMemoriaCompartidaEspiaProcesosMemoria;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartidaEspiaProcesosMemoria;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	/*----------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------------*/
	key_t keyEspiaProcesoPideMemoria; /*Clave que se pasa al shmget*/
	int idMemoriaCompartidaEspiaProcesoPideMemoria;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartidaEspiaProcesoPideMemoria;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	/*----------------------------------------------------------------------------------------------------*/
	

	/*----------------------------------------------------------------------------------------------------*/
	key_t keyEspiaProcesosBloqueados; /*Clave que se pasa al shmget*/
	int idMemoriaCompartidaEspiaProcesosBloqueados;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartidaEspiaProcesosBloqueados;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	/*----------------------------------------------------------------------------------------------------*/
	
	/*----------------------------------------------------------------------------------------------------*/
	key_t keyEspiaProcesosMuertos; /*Clave que se pasa al shmget*/
	int idMemoriaCompartidaEspiaProcesosMuertos;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartidaEspiaProcesosMuertos;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	/*----------------------------------------------------------------------------------------------------*/
	
	/*----------------------------------------------------------------------------------------------------*/
	key_t keyEspiaProcesosTerminados; /*Clave que se pasa al shmget*/
	int idMemoriaCompartidaEspiaProcesosTerminados;  /*Con este id se puede tener el acceso a la memoria compartida*/ 
	int tamanioMemoriaCompartidaEspiaProcesosTerminados;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	/*----------------------------------------------------------------------------------------------------*/
	
	

	/*----------------------------------------------------------------------------------------------------*/
	key = 5559;
	tamanioMemoriaCompartida = tamanio * sizeof(struct Pagina);
	banderaMemoriaCompartida = IPC_CREAT;
	idMemoriaCompartida = shmget(key, tamanioMemoriaCompartida, banderaMemoriaCompartida | 0666);

	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartida < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida **\n");
		return -1;

	}

	printf("--- ID para memoria compartida: %i ---\n", idMemoriaCompartida);
	/*----------------------------------------------------------------------------------------------------*/
	

	/*----------------------------------------------------------------------------------------------------*/
	keyPID = 5000;
	tamanioMemoriaCompartidaPID = pid * sizeof(int*);
	idMemoriaCompartidaPID = shmget(keyPID, tamanioMemoriaCompartidaPID, banderaMemoriaCompartida | 0666);

	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartidaPID < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida para el PID **\n");
		return -1;

	}
	printf("--- ID para memoria compartida PID: %i ---\n", idMemoriaCompartidaPID);	
	/*----------------------------------------------------------------------------------------------------*/


	/*----------------------------------------------------------------------------------------------------*/
	keyBitacora = 5555;
	tamanioMemoriaCompartidaBitacora = tamanioBitacora;
	idMemoriaCompartidaBitacora = shmget(keyBitacora, tamanioMemoriaCompartidaBitacora, banderaMemoriaCompartida | 0666);
	
	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartidaBitacora < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida para bitácora **\n");
		return -1;

	}
	printf("--- ID para memoria compartida bitácora: %i ---\n", idMemoriaCompartidaBitacora);
	
	/*----------------------------------------------------------------------------------------------------*/
	

	/*----------------------------------------------------------------------------------------------------*/
	keyEspiaProcesosMemoria = 5112;
	tamanioMemoriaCompartidaEspiaProcesosMemoria = tamanioEspiaProcesosMemoria *sizeof(struct ProcesoGeneral);

	idMemoriaCompartidaEspiaProcesosMemoria = shmget(keyEspiaProcesosMemoria, tamanioMemoriaCompartidaEspiaProcesosMemoria, banderaMemoriaCompartida | 0666);
	
	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartidaEspiaProcesosMemoria < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida para procesos en memoria **\n");
		return -1;

	}

	printf("--- ID para memoria compartida procesos en memoria: %i ---\n", idMemoriaCompartidaEspiaProcesosMemoria);
	
	/*----------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------------*/
	keyEspiaProcesoPideMemoria = 5333;
	tamanioMemoriaCompartidaEspiaProcesoPideMemoria = tamanioEspiaProcesoPideMemoria *sizeof(struct ProcesoGeneral);
	idMemoriaCompartidaEspiaProcesoPideMemoria = shmget(keyEspiaProcesoPideMemoria, tamanioMemoriaCompartidaEspiaProcesoPideMemoria, banderaMemoriaCompartida | 0666);
	
	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartidaEspiaProcesoPideMemoria < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida para proceso pide memoria **\n");
		return -1;

	}

	printf("--- ID para memoria compartida proceso pide memoria: %i ---\n", idMemoriaCompartidaEspiaProcesoPideMemoria);
	
	/*----------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------------*/
	keyEspiaProcesosBloqueados = 5144;
	tamanioMemoriaCompartidaEspiaProcesosBloqueados = tamanioEspiaProcesosBloqueados *sizeof(struct ProcesoGeneral);

	idMemoriaCompartidaEspiaProcesosBloqueados = shmget(keyEspiaProcesosBloqueados, tamanioMemoriaCompartidaEspiaProcesosBloqueados, banderaMemoriaCompartida | 0666);
	
	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartidaEspiaProcesosBloqueados < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida para procesos bloqueados **\n");
		return -1;

	}

	printf("--- ID para memoria compartida procesos bloqueados: %i ---\n", idMemoriaCompartidaEspiaProcesosBloqueados);
	
	/*----------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------------*/
	keyEspiaProcesosMuertos = 5452;
	tamanioMemoriaCompartidaEspiaProcesosMuertos = tamanioEspiaProcesosMuertos *sizeof(struct ProcesoGeneral);

	idMemoriaCompartidaEspiaProcesosMuertos = shmget(keyEspiaProcesosMuertos, tamanioMemoriaCompartidaEspiaProcesosMuertos, banderaMemoriaCompartida | 0666);
	
	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartidaEspiaProcesosMuertos < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida para procesosMuertos **\n");
		return -1;

	}

	printf("--- ID para memoria compartida procesos muertos: %i ---\n", idMemoriaCompartidaEspiaProcesosMuertos);
	
	/*----------------------------------------------------------------------------------------------------*/

	/*----------------------------------------------------------------------------------------------------*/
	keyEspiaProcesosTerminados = 5234;
	tamanioMemoriaCompartidaEspiaProcesosTerminados = tamanioEspiaProcesosTerminados *sizeof(struct ProcesoGeneral);

	idMemoriaCompartidaEspiaProcesosTerminados = shmget(keyEspiaProcesosTerminados, tamanioMemoriaCompartidaEspiaProcesosTerminados, banderaMemoriaCompartida | 0666);
	
	/*Se valida si se logra o no compartir la memoria*/
	if(idMemoriaCompartidaEspiaProcesosTerminados < 0){
		
		printf("** Error, no se pudo asignar la memoria compartida para procesos terminados **\n");
		return -1;

	}

	printf("--- ID para memoria compartida procesos terminados: %i ---\n", idMemoriaCompartidaEspiaProcesosTerminados);
	
	/*----------------------------------------------------------------------------------------------------*/
	

	
	

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

	//Para enseñar en una prueba
	
	/*
	paginas[0].disponible = 0;
	paginas[1].disponible = 0;
	paginas[6].disponible = 0;
	paginas[10].disponible = 0;
	paginas[11].disponible = 0;
	paginas[12].disponible = 0;
	paginas[13].disponible = 0;
	paginas[14].disponible = 0;
	paginas[15].disponible = 0;
	paginas[16].disponible = 0;
	paginas[17].disponible = 0;
	paginas[18].disponible = 0;
	paginas[19].disponible = 0;
	*/

	/*
	paginas[5].disponible = 0;
	paginas[9].disponible = 0;
	paginas[14].disponible = 0;
	paginas[15].disponible = 0;
	paginas[16].disponible = 0;
	paginas[17].disponible = 0;
	paginas[18].disponible = 0;
	paginas[19].disponible = 0;
	*/

	/*
	paginas[2].disponible = 0;
	paginas[5].disponible = 0;
	paginas[8].disponible = 0;
	paginas[11].disponible = 0;
	paginas[14].disponible = 0;
	paginas[17].disponible = 0;
	*/


	

	procesosEnMemoria = (ProcesoGeneral *) shmat(idMemoriaCompartidaEspiaProcesosMemoria, NULL, 0); /*Cast*/
	procesoPideMemoria = (ProcesoGeneral *) shmat(idMemoriaCompartidaEspiaProcesoPideMemoria, NULL, 0); /*Cast*/
	procesosBloqueados = (ProcesoGeneral *) shmat(idMemoriaCompartidaEspiaProcesosBloqueados, NULL, 0); /*Cast*/
	procesosMuertos = (ProcesoGeneral *) shmat(idMemoriaCompartidaEspiaProcesosMuertos, NULL, 0); /*Cast*/
	procesosTerminados = (ProcesoGeneral *) shmat(idMemoriaCompartidaEspiaProcesosTerminados, NULL, 0); /*Cast*/
	bitacora = (char *) shmat(idMemoriaCompartidaBitacora, NULL, 0);
	
	for(int i = 0; i<tamanioEspiaProcesosMemoria; i++){
		procesosEnMemoria[i].idProceso = -1;
		strcpy(procesosEnMemoria[i].tipoMecanismo, "n");
		procesosEnMemoria[i].idThread = -1;
	}

	procesoPideMemoria[0].idProceso = -1;
	strcpy(procesoPideMemoria[0].tipoMecanismo,"n");
	procesoPideMemoria[0].idThread = -1;

	for(int i = 0; i<tamanioEspiaProcesosBloqueados; i++){
		procesosBloqueados[i].idProceso = -1;
		strcpy(procesosBloqueados[i].tipoMecanismo,"n");
		procesosBloqueados[i].idThread  = -1;
	}

	for(int i = 0; i<tamanioEspiaProcesosMuertos; i++){
		procesosMuertos[i].idProceso = -1;
		strcpy(procesosMuertos[i].tipoMecanismo,"n");
		procesosMuertos[i].idThread = -1;
	}

	for(int i = 0; i<tamanioEspiaProcesosTerminados; i++){
		procesosTerminados[i].idProceso = -1;
		strcpy(procesosTerminados[i].tipoMecanismo,"n");
		procesosTerminados[i].idThread = -1;
	}


	
	guardarIdMemoriaCompartida(idMemoriaCompartida, "idMemoriaCompartida.txt");
	guardarIdMemoriaCompartida(idMemoriaCompartidaBitacora, "idMemoriaCompartidaBitacora.txt");
	guardarIdMemoriaCompartida(idMemoriaCompartidaEspiaProcesosMemoria, "procesosMemoria.txt");
	guardarIdMemoriaCompartida(idMemoriaCompartidaEspiaProcesoPideMemoria, "procesoPideMemoria.txt");
	guardarIdMemoriaCompartida(idMemoriaCompartidaEspiaProcesosBloqueados, "procesosBloqueados.txt");
	guardarIdMemoriaCompartida(idMemoriaCompartidaEspiaProcesosMuertos, "procesosMuertos.txt");
	guardarIdMemoriaCompartida(idMemoriaCompartidaEspiaProcesosTerminados, "procesosTerminados.txt");
	guardarIdMemoriaCompartida(idMemoriaCompartidaPID, "pid.txt");



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

	//
}