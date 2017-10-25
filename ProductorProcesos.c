/* Este programa estará encargado de pedir la memoria compartida
   incluyendo las páginas que requerirá para los procesos */


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
#include "ProcesoPaginacion.h"
#include "ProcesoSegmentacion.h"
#include "ProcesoGeneral.h"



#define tamanioPasoArchivo 100 /*Tamaño máximo para pasar el contenido del txt a una variable*/





int semaforo;
Pagina *paginas;
ProcesoGeneral *procesosEnMemoria;
ProcesoGeneral *procesoPideMemoria;
ProcesoGeneral *procesosBloqueados;
ProcesoGeneral *procesosMuertos;
ProcesoGeneral *procesosTerminados;
int *pID;
char *contenidoBitacora;
char *contenidoEspia;


/*--------------------------------------  Semáforos binarios  --------------------------------------*/

/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/*Función que libera el semáforo*/
void doSignal(int semid, int numSem) {
    struct sembuf sops; //Signal
    sops.sem_num = numSem;
    sops.sem_op = 1;
    sops.sem_flg = 0;
 
    if (semop(semid, &sops, 1) == -1) {
        perror(NULL);
        error("Error al hacer Signal");
    }
}

/*Función que utiliza el semáforo*/ 
void doWait(int semid, int numSem) {
    struct sembuf sops;
    sops.sem_num = numSem; /* Sobre el primero, ... */
    sops.sem_op = -1; /* ... un wait (resto 1) */
    sops.sem_flg = 0;
 
    if (semop(semid, &sops, 1) == -1) {
        perror(NULL);
        error("Error al hacer el Wait");
    }
}
 
/*Función que inicializa el semáforo*/
void initSem(int semid, int numSem, int valor) { //iniciar un semaforo
  
    if (semctl(semid, numSem, SETVAL, valor) < 0) {        
    perror(NULL);
        error("Error iniciando semaforo");
    }
}


void escribirEnBitacora(char *contenido){

	strcat(contenidoBitacora, contenido);

}

void escribirEnEspia(char *contenido){

	strcat(contenidoEspia, contenido);

}


/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/*--------------------------------------  Escritura en memoria compartida para el espía   --------------------------------------*/

/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */

void escribirProcesoEnMemoria(int idProceso, char tipo[12], unsigned long idThread){

	for(int i = 0; i<tamanioEspiaProcesosMemoria; i++){

		if(procesosEnMemoria[i].idProceso == -1){
			procesosEnMemoria[i].idProceso = idProceso;
			strcpy(procesosEnMemoria[i].tipoMecanismo,tipo);
			procesosEnMemoria[i].idThread = idThread;
			break;
		}

	}

}

void liberarProcesoEnMemoria(int idProceso){

	for(int i = 0; i<tamanioEspiaProcesosMemoria; i++){

		if(procesosEnMemoria[i].idProceso == idProceso){
			procesosEnMemoria[i].idProceso = -1;
			strcpy(procesosEnMemoria[i].tipoMecanismo, "n");
			procesosEnMemoria[i].idThread = -1;

		}
	}

}

void liberarProcesoPideMemoria(){

	procesoPideMemoria[0].idProceso = -1;
	strcpy(procesoPideMemoria[0].tipoMecanismo, "n");
	procesoPideMemoria[0].idThread = -1;

}

void escribirProcesoPideMemoria(int idProceso, char tipo[12], unsigned long idThread){

	procesoPideMemoria[0].idProceso = idProceso;
	strcpy(procesoPideMemoria[0].tipoMecanismo, tipo);
	procesoPideMemoria[0].idThread = idThread;
	
}

void escribirProcesoBloquedo(int idProceso, char tipo[12], unsigned long idThread){

	for(int i = 0; i<tamanioEspiaProcesosBloqueados; i++){

		if(procesosBloqueados[i].idProceso == -1){
			procesosBloqueados[i].idProceso = idProceso;
			strcpy(procesosBloqueados[i].tipoMecanismo,tipo);
			procesosBloqueados[i].idThread = idThread;	
			break;
		}
	
	}

}

void liberarProcesoBloqueado(int idProceso){

	for(int i = 0; i<tamanioEspiaProcesosBloqueados; i++){

		if(procesosBloqueados[i].idProceso == idProceso){
			
			procesosBloqueados[i].idProceso = -1;
			strcpy(procesosBloqueados[i].tipoMecanismo, "n");
			procesosBloqueados[i].idThread = -1;

		}

	}

}

void escribirProcesoMuerto(int idProceso, char tipo[12], unsigned long idThread){

	for(int i = 0; i<tamanioEspiaProcesosMuertos; i++){

		if(procesosMuertos[i].idProceso == -1){
			procesosMuertos[i].idProceso = idProceso;
			strcpy(procesosMuertos[i].tipoMecanismo,tipo);
			procesosMuertos[i].idThread = idThread;
			break;
		}

	}

}

void escribirProcesoTerminado(int idProceso, char tipo[12], unsigned long idThread){

	for(int i = 0; i<tamanioEspiaProcesosTerminados; i++){

		if(procesosTerminados[i].idProceso == -1){
			procesosTerminados[i].idProceso = idProceso;
			strcpy(procesosTerminados[i].tipoMecanismo,tipo);
			procesosTerminados[i].idThread = idThread;
			break;
		}

	}

}



/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */



/*--------------------------------------  Paginación  --------------------------------------*/


/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/* Función encargada de solicitar espacio en la memoria compartida para el proceso en el mecanismo de paginación */
int solicitarMemoriaPaginacion(int numeroPaginas){
	
	int contador; /*Para saber cuántos espacios libres hay*/
	char *mensaje;

	mensaje = (char*)malloc(sizeof(char*));
	
	contador = 0;

	printf("-----------------------------------------------------------------------\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");
	printf("\n\n");

	sprintf(mensaje, "-----------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "Los que tienen -1 y 1, son espacios libres\n\n\n");


	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado);
		sprintf(mensaje +strlen(mensaje), "Página: ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].numeroPagina);
		sprintf(mensaje +strlen(mensaje), " Disponible: ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].disponible);
		sprintf(mensaje +strlen(mensaje), " Número de proceso: ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].procesoOcupado);
		sprintf(mensaje +strlen(mensaje), "\n");
		
		/*Se pregunta si hay espacio disponible, en caso de que sí, se suma el contador para saber cuántas páginas hay*/
		if(paginas[i].disponible == 1) contador++;

	}
	printf("\n");
	printf("--- Páginas disponibles: %i ---\n", contador);

	sprintf(mensaje +strlen(mensaje), "\n--- Páginas disponibles: ");
	sprintf(mensaje +strlen(mensaje), "%d", contador);
	sprintf(mensaje +strlen(mensaje), "---\n");
	
	if(contador >= numeroPaginas){

		printf("--- ¡Hay suficientes páginas para ejecutar este proceso! :) --- \n");
		printf("\n\n");
		printf("-----------------------------------------------------------------------\n");
		printf("\n\n");

		sprintf(mensaje +strlen(mensaje), "--- ¡Hay suficientes páginas para ejecutar este proceso! :) --- \n");
		sprintf(mensaje +strlen(mensaje), "\n\n");
		sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n");
		sprintf(mensaje +strlen(mensaje), "\n\n");

		escribirEnBitacora(mensaje);
		mensaje = (char*)malloc(sizeof(char*));
	

		
		return 1; /*Hay suficientes páginas*/

	} 
	
	else{

		printf("--- ¡No hay suficientes páginas para ejecutar este proceso! :( --- \n");
		printf("\n\n");
		printf("-----------------------------------------------------------------------\n");
		printf("\n\n");

		sprintf(mensaje +strlen(mensaje), "--- ¡No hay suficientes páginas para ejecutar este proceso! :( --- \n");
		sprintf(mensaje +strlen(mensaje), "\n\n");
		sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n");
		sprintf(mensaje +strlen(mensaje), "\n\n");

		escribirEnBitacora(mensaje);
		mensaje = (char*)malloc(sizeof(char*));

		return 0; /*No hay suficientes páginas*/
		
	} 


}

/*Función encargada de ocupar las páginas con procesos, en el mecanismo de paginación*/
void tomarMemoriaPaginacion(int numeroPaginas, int idProceso){

	int contador; /*Para delimitar cuando se ocupen las páginas que se requieren*/

	char *mensaje;

	mensaje = (char*)malloc(sizeof(char*));

	contador = 0;

	for(int i = 0; i<tamanio; i++){

		/*Pregunto si hay espacios disponibles*/
		if(paginas[i].disponible == 1){

			 paginas[i].procesoOcupado = idProceso;
			 paginas[i].disponible = 0;

			 /*Se lleva un contador para saber cuándo parar con la asignación de páginas*/
			 contador++;

		}
		
		/*Si el contador llega al número de páginas, entonces se habrá asignado lo necesario*/
		if(contador == numeroPaginas) break;

	}	

	printf("--- Se asignaron las páginas para el proceso: %i ---\n", idProceso); /*Asignación*/
	printf("--- Así se ve la memoria ahora: ---\n");
	printf("\n\n");

	printf("-----------------------------------------------------------------------\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");

	sprintf(mensaje, "--- Se asignaron las páginas para el proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "---\n");
	sprintf(mensaje +strlen(mensaje), "\n\n");
	sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "Los que tienen -1 y 1, son espacios libres\n");


	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado);
		
		sprintf(mensaje +strlen(mensaje), "Página: ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].numeroPagina);
		sprintf(mensaje +strlen(mensaje), " Disponible: ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].disponible);
		sprintf(mensaje +strlen(mensaje), " Número de proceso: ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].procesoOcupado);
		sprintf(mensaje +strlen(mensaje), "\n");
	}

	printf("-----------------------------------------------------------------------\n");
	printf("\n\n");

	sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "\n\n");
	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));



} 

/* Función encargada de devolver el espacio en la memoria compartida para el proceso en el mecanismo de paginación */
void liberarMemoriaPaginacion(int idProceso){

	char *mensaje;
	mensaje = (char*)malloc(sizeof(char*));

	printf("--- Se liberarán las páginas para el proceso: %i ---\n", idProceso); /*Desasignación*/

	sprintf(mensaje, "--- Se liberarán las páginas para el proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "---\n");
	
	

	for(int i = 0; i<tamanio; i++){

		/*Pregunta si un campo ocupado coincide con el id que se desea sacar*/
		if(paginas[i].procesoOcupado == idProceso && paginas[i].disponible == 0){ 

			paginas[i].procesoOcupado = -1; //No le queda ningún proceso asignado
			paginas[i].disponible = 1; //Vuelve a estar disponible

		}

	}

	printf("--- Se han liberado las páginas para el proceso: %i ---\n", idProceso);

	sprintf(mensaje, "--- Se han liberado las páginas para el proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "---\n");

	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));
	
	

}

/*Función encargada de ejecutar el proceso en el modelo de paginación, tiene como parámetro el proceso, con las páginas que ocupa y el tiempo que se ejecutará*/
int *ejecucionProcesoPaginacion(void *proceso){
	/*FALTA ESCRIBIR EN LA BITÁCORA*/	

	int aceptaSolicitudMemoria; /*Para saber si hay espacio suficiente en memoria*/
	int idProceso;
	int cantidadPaginas;
	int tiempo;
	unsigned long idThread;
	char *mensaje;

	ProcesoPaginacion *procesoP;

	procesoP = (ProcesoPaginacion*) proceso; /*Cast*/
	
	procesoP->idThread = pthread_self();
	idThread = pthread_self();
	
	idProceso = procesoP->idProceso;
	cantidadPaginas = procesoP->cantidadPaginas;
	tiempo = procesoP->tiempo;
	mensaje = (char*)malloc(sizeof(char*));
		


	printf("--------------------- Proceso: %i --------------------- \n", idProceso);

	printf("----------------------------------------------------------------------------------------------------------------------\n");
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	
	printf("\n\n");

	printf("------------ Esperando para usar el semáforo ------------ \n");
	
	sprintf(mensaje, "--------------------- Proceso: ");
	sprintf(mensaje + strlen(mensaje), "%d", idProceso);
	sprintf(mensaje + strlen(mensaje), "--------------------- \n");
	sprintf(mensaje + strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje + strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje + strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje + strlen(mensaje), "\n\n");
	sprintf(mensaje + strlen(mensaje), "------------ Esperando para usar el semáforo ------------ \n");

	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));
	

	//printf("MENSAJEEEEE: %s\n", mensaje);


	escribirProcesoBloquedo(idProceso, "Paginacion", idThread); /*Espía*/
	
	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	liberarProcesoBloqueado(idProceso); /*Espía*/

	printf("------------ Entra al semaforo               ------------\n");

	

	printf("------------ Páginas requeridas: %i          ------------\n", cantidadPaginas);
	printf("\n\n");

	sprintf(mensaje, "------------ Entra al semaforo               ------------\n");
	sprintf(mensaje +strlen(mensaje), "------------ Páginas requeridas: ");
	sprintf(mensaje +strlen(mensaje), "%d", cantidadPaginas);
	sprintf(mensaje +strlen(mensaje), "------------\n\n\n");

	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));
	
	escribirProcesoPideMemoria(idProceso, "Paginacion", idThread);/*Espía*/
	
	aceptaSolicitudMemoria = solicitarMemoriaPaginacion(procesoP->cantidadPaginas);
	
	/*Si no hay suficiente espacio, se mata el proceso y se devuelve el semáforo*/
	if(aceptaSolicitudMemoria == 0){

		printf("Matando al proceso: %i\n", idProceso);
		printf("----------------------------------------------------------------------------------------------------------------------\n");
		printf("----------------------------------------------------------------------------------------------------------------------\n");
		printf("----------------------------------------------------------------------------------------------------------------------\n");
		printf("\n\n");

		sprintf(mensaje, "Matando al proceso: ");
		sprintf(mensaje +strlen(mensaje), "%d", idProceso);
		sprintf(mensaje +strlen(mensaje), "\n");
		sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
		sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
		sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
		sprintf(mensaje +strlen(mensaje), "\n\n");

		escribirEnBitacora(mensaje);
		mensaje = (char*)malloc(sizeof(char*));
	



		escribirProcesoMuerto(idProceso, "Paginacion", idThread);
		doSignal(semaforo,0); /*Libera el semáforo*/

		return -1;	
	}


	/*Se ocupan los campos en memoria*/
	tomarMemoriaPaginacion(procesoP->cantidadPaginas, procesoP->idProceso);

	escribirProcesoEnMemoria(idProceso, "Paginacion", idThread); /*Espía*/

	liberarProcesoPideMemoria();

	sprintf(mensaje, "----- Comienza el sleep del proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "-----\n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "\n\n");

	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));
	
	
	doSignal(semaforo,0); /*Libera semáforo*/


	
	/*Escribir en bitácora*/
	printf("----- Comienza el sleep del proceso: %i -----\n", idProceso);
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	printf("\n\n");

	sleep(10);
	//sleep(tiempo);

	printf("----- Termina el sleep del proceso: %i -----\n", idProceso);
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	printf("\n\n");

	

	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	sprintf(mensaje, "----- Termina el sleep del proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "-----\n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "\n\n");

	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));
	

	liberarMemoriaPaginacion(idProceso);	

	liberarProcesoEnMemoria(idProceso); /*Espía*/

	escribirProcesoTerminado(idProceso, "Paginacion", idThread);

	sprintf(mensaje, "--- Terminó la ejecución de este proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "---\n\n\n");

	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));
	
	
	doSignal(semaforo,0); /*Libera el semáforo*/

	

	printf("--- Terminó la ejecución de este proceso: %i ---\n", idProceso);
	printf("\n\n");

	return 1;
}

/*Función encargada de crear procesos para el mecanismo de paginación*/
void crearHiloPaginacion(ProcesoPaginacion *proceso){

	int ret;
	int idCreacionHilo;
	pthread_attr_t tattr;
	pthread_t sniffer_thread;


	
	/*Se inicializan los valores del hilo*/
	ret = pthread_attr_init(&tattr);
	//cambia el detachstate para liberar recursos cuando termine el hilo
	ret = pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
							
	
	idCreacionHilo = pthread_create( &sniffer_thread , &tattr ,  &ejecucionProcesoPaginacion ,  proceso);	
	

	if( proceso->idProceso < 0)
	{
		perror("No se pudo crear el hilo correctamente");
		
	}

}


/*Función encargada de actuar como el mecanismo de paginación*/
void mecanismoPaginacion(){
	
	//char temporal[10];
		
	char *mensaje;

	semaforo=semget(IPC_PRIVATE,1,IPC_CREAT | 0700);

	
	if(semaforo < 0){

		printf("Error con el semáforo");
		return -1;

	}
	
	initSem(semaforo,0,1);

	int hora; /*Para la función srand*/
	int cantidadPaginas; 
	int cantidadSegundos;  /*Segundos que durará ejecutándose el proceso*/
	int cantidadSegundosPorProceso;  /*Cada cuanto se hará un proceso*/
	int numeroProcesos;
	ProcesoPaginacion *proceso;

  
	hora = time(NULL);  
    // Semilla de rand();
    srand(hora);

    
   	numeroProcesos = 0;

   	
    /*La idea, por ahora, es crear 1000 procesos que pidan memoria en la compartida*/
    while(numeroProcesos < 1000){

    	

    	
    	mensaje = (char*)malloc(sizeof(char*));
	
		

    	//31 + 30
    	cantidadSegundosPorProceso = rand()% 30 + 30;  /*Random entre 30 y 60*/ 
    	cantidadPaginas = rand()% 10 + 1;   /*Random entre 1 y 10*/
    	cantidadSegundos = rand()% 40 + 20; /*Random entre 20 y 60*/

    	printf("--- ¡Se va a generar un nuevo proceso! ---\n");
    	printf("-------------------------------------------------\n");
    	

    	printf("-------------------------------------------------\n");
    	printf("Páginas para el proceso: %i\n", cantidadPaginas);
    	printf("Segundos para el proceso: %i\n", cantidadSegundos);	
    	printf("-------------------------------------------------\n");
    	

    	proceso->idProceso = numeroProcesos;
    	proceso->cantidadPaginas = cantidadPaginas;
    	proceso->tiempo = cantidadSegundos;


    	numeroProcesos++;

    	printf("Tiempo hasta el próximo proceso: %i\n", cantidadSegundosPorProceso);
    	printf("-------------------------------------------------\n");
    	printf("\n\n");

    	

    	
    	sprintf(mensaje, "--- ¡Se va a generar un nuevo proceso! ---\n");
    	sprintf(mensaje +strlen(mensaje), "-------------------------------------------------\n");
    	sprintf(mensaje +strlen(mensaje), "-------------------------------------------------\n");
    	sprintf(mensaje +strlen(mensaje), "Páginas para el proceso: ");
    	
    	sprintf(mensaje +strlen(mensaje), "%d", cantidadPaginas);
    	
    	
    	sprintf(mensaje +strlen(mensaje), "\n");
    	sprintf(mensaje +strlen(mensaje), "Segundos para el proceso: ");
    	sprintf(mensaje +strlen(mensaje), "%d", cantidadSegundos);
    	sprintf(mensaje +strlen(mensaje), "\n");
    	sprintf(mensaje +strlen(mensaje), "-------------------------------------------------\n");
    	sprintf(mensaje +strlen(mensaje), "Tiempo hasta el próximo proceso: ");
    	sprintf(mensaje +strlen(mensaje), "%d", cantidadSegundosPorProceso);
    	sprintf(mensaje +strlen(mensaje), "\n");
    	sprintf(mensaje +strlen(mensaje), "-------------------------------------------------\n\n\n");

    	/*Después de esto, escribir en la bitácora*/
    	
    	
    	//printf("MSJ: %s\n", mensaje);
    	

    	escribirEnBitacora(mensaje);
    	
    	crearHiloPaginacion(proceso);
    	//sleep(cantidadSegundosPorProceso);
    	sleep(2);

    }	

}


/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */





/*--------------------------------------  Segmentación  --------------------------------------*/


/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/*Función encargada de calcular el número de páginas totales requeridas para un proceso en el mecanismo de segmentación*/
int calcularTotalPaginasProceso(ProcesoSegmentacion *proceso){

	int numeroSegmentos; 
	int paginasTotalesProceso;

	numeroSegmentos = proceso->numeroSegmentos;

	paginasTotalesProceso = 0;

	for(int i = 0; i<numeroSegmentos; i++){

		paginasTotalesProceso+=proceso->segmentos[i].numeroPaginas;


	}

	printf("Total de páginas para el proceso: %i\n", paginasTotalesProceso);
	
	return paginasTotalesProceso;

}

int apartarPaginasContiguas(int numeroPaginas, int idProceso, int segmento){


	int contador; /*Para saber cuántos espacios libres hay*/

	int marca; /*Para saber dónde empieza una serie de bloque contiguos*/
	int marcaAux;
	
	char *mensaje;

	mensaje = (char*)malloc(sizeof(char*));
	
	
	contador = 0;
	marca = 0;
	marcaAux;

	marcaAux = -1;

	printf("--- Buscando espacios contiguos para el proceso: %i segmento: %i páginas requeridas: %i ---\n", idProceso, segmento, numeroPaginas);
	printf("-----------------------------------------------------------------------------------------\n");
	printf("--- Así se ve la memoria ahora ---\n");
	printf("-----------------------------------------------------------------------\n");
	printf("\n\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");


	sprintf(mensaje, "--- Buscando espacios contiguos para el proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), " segmento: ");
	sprintf(mensaje +strlen(mensaje), "%d", segmento);
	sprintf(mensaje +strlen(mensaje), " páginas requeridas: ");
	sprintf(mensaje +strlen(mensaje), "%d", numeroPaginas);
	sprintf(mensaje +strlen(mensaje), " ---\n");

	sprintf(mensaje +strlen(mensaje), "--- Así se ve la memoria ahora ---\n");
	sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n\n\n");
	sprintf(mensaje +strlen(mensaje), "Los que tienen -1 y 1, son espacios libres\n\n\n");




	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i Segmento: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado, paginas[i].numeroSegmento);
		
		sprintf(mensaje +strlen(mensaje), "Página ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].numeroPagina);
		sprintf(mensaje +strlen(mensaje), " Disponible ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].disponible);
		sprintf(mensaje +strlen(mensaje), " Número de proceso ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].procesoOcupado);
		sprintf(mensaje +strlen(mensaje), " Segmento ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].numeroSegmento);
		sprintf(mensaje +strlen(mensaje), "\n");


	}
	printf("\n\n");
	printf("-----------------------------------------------------------------------\n");

	sprintf(mensaje +strlen(mensaje), "\n\n");
	sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n");
	
	
	for(int i = 0; i<tamanio; i++){
		
		/*Se pregunta si hay espacio disponible, en caso de que sí, se suma el contador para saber cuántas páginas hay*/
		if(paginas[i].disponible == 1){ 
			
			contador++;
		
		}

		else{
			

			if(contador == numeroPaginas){
				printf("--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: %i para el segmento: %i comienza en la celda: %i! :) ---\n", numeroPaginas, segmento, marca);
				printf("-----------------------------------------------------------------------------------------\n");
				printf("\n\n");

				sprintf(mensaje +strlen(mensaje), "--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: ");
				sprintf(mensaje +strlen(mensaje), "%d", numeroPaginas);
				sprintf(mensaje +strlen(mensaje), " para el segmento: ");
				sprintf(mensaje +strlen(mensaje), "%d", segmento);
				sprintf(mensaje +strlen(mensaje), " comienza en la celda: ");
				sprintf(mensaje +strlen(mensaje), "%d", marca);
				sprintf(mensaje +strlen(mensaje), "! :) ---\n");
				sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------------------------\n\n\n");
				escribirEnBitacora(mensaje);

				return marca;
			}

			else if(contador > numeroPaginas){

				marcaAux = marca;
				marca = i+1;
				contador = 0;

			}

			else if(contador < numeroPaginas){

				marca = i+1;
				contador = 0;
			}
			/*if(contador >= numeroPaginas){
				printf("--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: %i para el segmento: %i comienza en la celda: %i! :) ---\n", numeroPaginas, segmento, marca);
				printf("-----------------------------------------------------------------------------------------\n");
				printf("\n\n");

				sprintf(mensaje +strlen(mensaje), "--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: ");
				sprintf(mensaje +strlen(mensaje), "%d", numeroPaginas);
				sprintf(mensaje +strlen(mensaje), " para el segmento: ");
				sprintf(mensaje +strlen(mensaje), "%d", segmento);
				sprintf(mensaje +strlen(mensaje), " comienza en la celda: ");
				sprintf(mensaje +strlen(mensaje), "%d", marca);
				sprintf(mensaje +strlen(mensaje), "! :) ---\n");
				sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------------------------\n\n\n");
				escribirEnBitacora(mensaje);
	
				return marca; //Retorna el índice de la página donde empiezan contiguas

			}

			else{
				
				marca = i+1;
				contador = 0;	
			
			}*/ 
		
		}

	}

	if(contador >= numeroPaginas){
		
		printf("--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: %i para el segmento: %i comienza en la celda: %i! :) ---\n", numeroPaginas, segmento, marca);
		printf("-----------------------------------------------------------------------------------------\n");
		printf("\n\n");

		sprintf(mensaje +strlen(mensaje), "--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: ");
		sprintf(mensaje +strlen(mensaje), "%d", numeroPaginas);
		sprintf(mensaje +strlen(mensaje), " para el segmento: ");
		sprintf(mensaje +strlen(mensaje), "%d", segmento);
		sprintf(mensaje +strlen(mensaje), " comienza en la celda: ");
		sprintf(mensaje +strlen(mensaje), "%d", marca);
		sprintf(mensaje +strlen(mensaje), "! :) ---\n");
		sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------------------------\n\n\n");
		escribirEnBitacora(mensaje);
	
		return marca; /*Retorna el índice de la página donde empiezan contiguas*/

	}

	else{

		if(marcaAux == -1){
			printf("--- ¡No se logró encontrar un espacio con páginas contiguas de tamaño: %i para el segmento: %i :(! ---\n", numeroPaginas, segmento);
			printf("-----------------------------------------------------------------------------------------\n");
			printf("\n\n");

			sprintf(mensaje +strlen(mensaje), "--- ¡No se logró encontrar un espacio con páginas contiguas de tamaño: ");
			sprintf(mensaje +strlen(mensaje), "%d", numeroPaginas);
			sprintf(mensaje +strlen(mensaje), " para el segmento: ");
			sprintf(mensaje +strlen(mensaje), "%d", segmento);
			sprintf(mensaje +strlen(mensaje), " :(! ---\n");
			sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------------------------\n\n\n");
	
			escribirEnBitacora(mensaje);

			return -1;	

		}

		else{

			printf("--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: %i para el segmento: %i comienza en la celda: %i! :) ---\n", numeroPaginas, segmento, marcaAux);
			printf("-----------------------------------------------------------------------------------------\n");
			printf("\n\n");

			sprintf(mensaje +strlen(mensaje), "--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: ");
			sprintf(mensaje +strlen(mensaje), "%d", numeroPaginas);
			sprintf(mensaje +strlen(mensaje), " para el segmento: ");
			sprintf(mensaje +strlen(mensaje), "%d", segmento);
			sprintf(mensaje +strlen(mensaje), " comienza en la celda: ");
			sprintf(mensaje +strlen(mensaje), "%d", marca);
			sprintf(mensaje +strlen(mensaje), "! :) ---\n");
			sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------------------------\n\n\n");
			escribirEnBitacora(mensaje);

			return marcaAux;
		}

		/*
		printf("--- ¡No se logró encontrar un espacio con páginas contiguas de tamaño: %i para el segmento: %i :(! ---\n", numeroPaginas, segmento);
		printf("-----------------------------------------------------------------------------------------\n");
		printf("\n\n");

		sprintf(mensaje +strlen(mensaje), "--- ¡No se logró encontrar un espacio con páginas contiguas de tamaño: ");
		sprintf(mensaje +strlen(mensaje), "%d", numeroPaginas);
		sprintf(mensaje +strlen(mensaje), " para el segmento: ");
		sprintf(mensaje +strlen(mensaje), "%d", segmento);
		sprintf(mensaje +strlen(mensaje), " :(! ---\n");
		sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------------------------\n\n\n");
	
		escribirEnBitacora(mensaje);
		*/
		
	
		
	
	} 


}

int asignarContiguas(int idProceso, int cantidadPaginas, int marca, int numeroSegmento){

	
	int marcaAux;
	char *mensaje;
	marcaAux = marca;
	
	mensaje = (char*)malloc(sizeof(char*));
	
	printf("--- Asignando espacios contiguos para el proceso: %i segmento: %i páginas requeridas: %i ---\n", idProceso, numeroSegmento, cantidadPaginas);
	
	printf("--------------------------------------------------------------------------------------------------------\n");
	

	sprintf(mensaje, "--- Asignando espacios contiguos para el proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), " segmento: ");
	sprintf(mensaje +strlen(mensaje), "%d", numeroSegmento);
	sprintf(mensaje +strlen(mensaje), " páginas requeridas: ");
	sprintf(mensaje +strlen(mensaje), "%d", cantidadPaginas);
	sprintf(mensaje +strlen(mensaje), " ---\n");
	for(int i = 0; i<cantidadPaginas; i++){
		
		paginas[marcaAux].procesoOcupado = idProceso;
		paginas[marcaAux].disponible = 0;
		paginas[marcaAux].numeroSegmento = numeroSegmento;
		//paginas[marcaAux].numeroSegmento = numeroSegmento;
		marcaAux++;

	}

	printf("--- Así se vería la memoria si se lograra la asignación, faltan los demás segmentos ---\n");
	printf("-----------------------------------------------------------------------\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");
	printf("\n\n");

	sprintf(mensaje +strlen(mensaje), "--- Así se vería la memoria si se lograra la asignación, faltan los demás segmentos ---\n");
	sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "Los que tienen -1 y 1, son espacios libres\n\n\n");

	for(int i = 0; i<tamanio; i++){
		printf("Página %i: Disponible: %i Número de proceso: %i Segmento: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado, paginas[i].numeroSegmento);
	
		sprintf(mensaje +strlen(mensaje), "Página ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].numeroPagina);
		sprintf(mensaje +strlen(mensaje), " Disponible ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].disponible);
		sprintf(mensaje +strlen(mensaje), " Número de proceso ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].procesoOcupado);
		sprintf(mensaje +strlen(mensaje), " Segmento ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].numeroSegmento);
		sprintf(mensaje +strlen(mensaje), "\n");

	}
	printf("\n\n");
	printf("-----------------------------------------------------------------------\n");
	printf("--------------------------------------------------------------------------------------------------------\n");
	
	sprintf(mensaje +strlen(mensaje), "\n\n");
	sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "--------------------------------------------------------------------------------------------------------\n");


	escribirEnBitacora(mensaje);
	return 0;


}


/* Función encargada de solicitar espacio en la memoria compartida para el proceso en el mecanismo de segmentación */
int solicitarMemoriaSegmentacion(ProcesoSegmentacion *proceso){

	int paginasTotalesProceso;
	paginasTotalesProceso = calcularTotalPaginasProceso(proceso);

	int contador; /*Para saber cuántos espacios libres hay*/

	char *mensaje;

	mensaje = (char*)malloc(sizeof(char*));
	
	contador = 0;

	printf("------------ Páginas requeridas: %i          ------------\n", paginasTotalesProceso);
	printf("\n");

	printf("-----------------------------------------------------------------------\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");
	printf("\n\n");

	sprintf(mensaje, "------------ Páginas requeridas: ");
	sprintf(mensaje +strlen(mensaje), "%d", paginasTotalesProceso);
	sprintf(mensaje +strlen(mensaje), "------------\n\n");
	sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "Los que tienen -1 y 1, son espacios libres\n\n\n");

	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i Segmento: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado, paginas[i].numeroSegmento);
		
		sprintf(mensaje +strlen(mensaje), "Página ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].numeroPagina);
		sprintf(mensaje +strlen(mensaje), " Disponible ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].disponible);
		sprintf(mensaje +strlen(mensaje), " Número de proceso ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].procesoOcupado);
		sprintf(mensaje +strlen(mensaje), " Segmento ");
		sprintf(mensaje +strlen(mensaje), "%d", paginas[i].numeroSegmento);
		sprintf(mensaje +strlen(mensaje), "\n");
		

		/*Se pregunta si hay espacio disponible, en caso de que sí, se suma el contador para saber cuántas páginas hay*/
		if(paginas[i].disponible == 1) contador++;

	}

	printf("--- Páginas disponibles: %i ---\n", contador);

	sprintf(mensaje +strlen(mensaje), "--- Páginas disponibles: ");
	sprintf(mensaje +strlen(mensaje), "%d", contador);
	sprintf(mensaje +strlen(mensaje), "---\n");
	if(contador >= paginasTotalesProceso){

		printf("--- ¡Hay suficientes páginas para ejecutar este proceso! :) --- \n");
		printf("\n\n");
		printf("-----------------------------------------------------------------------\n"); 
		printf("\n\n");

		sprintf(mensaje +strlen(mensaje), "--- ¡Hay suficientes páginas para ejecutar este proceso! :) --- \n\n\n");
		sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n\n\n");
		escribirEnBitacora(mensaje);
		return 1; /*Hay suficientes páginas*/
	}
	
	else{
		printf("--- ¡No hay suficientes páginas para ejecutar este proceso! :( --- \n");
		printf("\n\n");
		printf("-----------------------------------------------------------------------\n"); 
		printf("\n\n");

		sprintf(mensaje +strlen(mensaje), "--- ¡No hay suficientes páginas para ejecutar este proceso! :( --- \n\n\n");
		sprintf(mensaje +strlen(mensaje), "-----------------------------------------------------------------------\n\n\n");
		escribirEnBitacora(mensaje);
		return 0; /*No hay suficientes páginas*/
	}

}

/* Función encargada de devolver el espacio en la memoria compartida para el proceso en el mecanismo de segmentación */
void liberarMemoriaSegmentacion(int idProceso){

	char *mensaje;

	mensaje = (char*)malloc(sizeof(char*));
	

	printf("--- Liberando segmentos para el proceso: %i ---\n", idProceso);

	sprintf(mensaje, "--- Liberando segmentos para el proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "---\n");

	//escribirEnBitacora(mensaje);
	//mensaje = (char*)malloc(sizeof(char*));
	

	for(int i = 0; i<tamanio; i++){

		/*Pregunta si un campo ocupado coincide con el id que se desea sacar*/
		if(paginas[i].procesoOcupado == idProceso && paginas[i].disponible == 0){ 

			paginas[i].procesoOcupado = -1; //No le queda ningún proceso asignado
			paginas[i].disponible = 1; //Vuelve a estar disponible
			paginas[i].numeroSegmento = -1;


		}

	}

	printf("--- Se han liberado los segmentos para el proceso: %i ---\n", idProceso);

	sprintf(mensaje +strlen(mensaje), "--- Se han liberado los segmentos para el proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "---\n");
	escribirEnBitacora(mensaje);
	//mensaje = (char*)malloc(sizeof(char*));
	
}





/*Función encargada de ejecutar el proceso en el modelo de segmentación*/
int *ejecucionProcesoSegmentacion(void *proceso){

	/*FALTA ESCRIBIR EN LA BITÁCORA*/	

	int aceptaSolicitudMemoria;
	int idProceso;
	int tiempo;
	int numeroSegmentos;
	int cantidadPaginas;
	char *mensaje;
	unsigned long idThread;

	ProcesoSegmentacion *procesoS;


	procesoS =  (ProcesoSegmentacion *) proceso; /*Cast*/
	
	procesoS->idThread = pthread_self();
	idThread = pthread_self();
	
	idProceso = procesoS->idProceso;
	tiempo = procesoS->tiempo;
	numeroSegmentos = procesoS->numeroSegmentos;  

	mensaje = (char*)malloc(sizeof(char*));
	
	
	printf("--------------------- Proceso: %i --------------------- \n", idProceso);

	printf("----------------------------------------------------------------------------------------------------------------------\n");
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	
	printf("\n\n");
	printf("------------ Esperando para usar el semáforo ------------ \n");
	
	sprintf(mensaje, "--------------------- Proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "--------------------- \n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
	sprintf(mensaje +strlen(mensaje), "\n\n");
	sprintf(mensaje +strlen(mensaje), "------------ Esperando para usar el semáforo ------------ \n");

	escribirEnBitacora(mensaje);

	mensaje = (char*)malloc(sizeof(char*));
	


	escribirProcesoBloquedo(idProceso, "Segmentacion", idThread); /*Espía*/
	
	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	liberarProcesoBloqueado(idProceso); /*Espía*/
	
	printf("------------ Entra al semaforo ------------\n");

	sprintf(mensaje +strlen(mensaje), "------------ Entra al semaforo ------------\n");
	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));
	

	escribirProcesoPideMemoria(idProceso, "Segmentacion", idThread); /*Espía*/

	aceptaSolicitudMemoria = solicitarMemoriaSegmentacion(procesoS);
	
	if(aceptaSolicitudMemoria == 0){

		printf("--- Matando al proceso: %i ---\n", idProceso);
		printf("----------------------------------------------------------------------------------------------------------------------\n");
		printf("----------------------------------------------------------------------------------------------------------------------\n");
		printf("----------------------------------------------------------------------------------------------------------------------\n");
		printf("\n\n");
		escribirProcesoMuerto(idProceso, "Segmentacion", idThread); /*Espía*/

		sprintf(mensaje, "--- Matando al proceso: ");
		sprintf(mensaje +strlen(mensaje), "%d", idProceso);
		sprintf(mensaje +strlen(mensaje), "---\n");
		sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
		sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
		sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n\n\n");
		escribirEnBitacora(mensaje);
		doSignal(semaforo,0); /*Libera el semáforo*/

		return -1;	
	}


	for(int i = 0; i<numeroSegmentos; i++){

		int numeroPaginas;
		int marca;
		int numeroSegmento;

		numeroPaginas = procesoS->segmentos[i].numeroPaginas;
		numeroSegmento = procesoS->segmentos[i].numeroSegmento;

		marca = apartarPaginasContiguas(numeroPaginas, idProceso, numeroSegmento);

		if(marca == -1){
			
			printf("--- Matando al proceso: %i ---\n", idProceso);

			liberarMemoriaSegmentacion(idProceso);
			printf("----------------------------------------------------------------------------------------------------------------------\n");
			printf("----------------------------------------------------------------------------------------------------------------------\n");
			printf("----------------------------------------------------------------------------------------------------------------------\n");
			printf("\n\n");
			escribirProcesoMuerto(idProceso, "Segmentacion", idThread); /*Espía*/

			sprintf(mensaje, "--- Matando al proceso: ");
			sprintf(mensaje +strlen(mensaje), "%d", idProceso);
			sprintf(mensaje +strlen(mensaje), "---\n");
			sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
			sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");
			sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n\n\n");
			escribirEnBitacora(mensaje);
			doSignal(semaforo,0); /*Libera el semáforo*/

			return -1;
		}

		else asignarContiguas(idProceso, numeroPaginas, marca, numeroSegmento);

	}

	//ESCRIBIR EN BITÁCORA
	escribirProcesoEnMemoria(idProceso, "Segmentacion", idThread); /*Espía*/
	
	liberarProcesoPideMemoria(); /*Espía*/

	sprintf(mensaje, "----- Comienza el sleep del proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "-----\n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");

	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));
	


	doSignal(semaforo,0);
	
	printf("----- Comienza el sleep del proceso: %i -----\n", idProceso);
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	


	sleep(5);
	//sleep(tiempo);
	
	
	printf("----- Termina el sleep del proceso: %i -----\n", idProceso);
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	

	
	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	sprintf(mensaje, "----- Termina el sleep del proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "-----\n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");

	escribirEnBitacora(mensaje);
	mensaje = (char*)malloc(sizeof(char*));
	


	liberarMemoriaSegmentacion(idProceso);

	//liberarProcesoEnMemoria(idProceso); /*Espía*/

	//ESCRIBIR EN BITÁCORA
	
	escribirProcesoTerminado(idProceso, "Segmentacion", idThread); /*Espía*/

	sprintf(mensaje, "--- Terminó la ejecución del proceso: ");
	sprintf(mensaje +strlen(mensaje), "%d", idProceso);
	sprintf(mensaje +strlen(mensaje), "---\n");
	sprintf(mensaje +strlen(mensaje), "----------------------------------------------------------------------------------------------------------------------\n");

	escribirEnBitacora(mensaje);

	mensaje = (char*)malloc(sizeof(char*));
	


	doSignal(semaforo,0); /*Libera el semáforo*/
			

	printf("--- Terminó la ejecución del proceso: %i ---\n", idProceso);
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	

	return 1;
	/*Aparentemente no se necesita ninguna línea de código para liberar al hilo, jejeps*/


}



/*Función encargada de crear procesos para el mecanismo de segmentación*/
void crearHiloSegmentacion(ProcesoSegmentacion *proceso){

	int ret;
	int idCreacionHilo;
	pthread_attr_t tattr;
	pthread_t sniffer_thread;
	
	/*Se inicializan los valores del hilo*/
	ret = pthread_attr_init(&tattr);
	//cambia el detachstate para liberar recursos cuando termine el hilo
	ret = pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
							
	idCreacionHilo = pthread_create( &sniffer_thread , &tattr ,  &ejecucionProcesoSegmentacion , proceso);	
							 
	if( idCreacionHilo < 0)
	{
		perror("No se pudo crear el hilo correctamente");
		
	}




}



/*Función encargada de actuar como el mecanismo de segmentación*/
void mecanismoSegmentacion(){
	
	semaforo=semget(IPC_PRIVATE,1,IPC_CREAT | 0700);
	
	if(semaforo < 0){

		printf("Error con el semáforo");
		return -1;

	}
	
	initSem(semaforo,0,1);

	int hora; /*Para la función srand*/
	int cantidadSegmentos; /*Segmentos que tendrá el proceso*/
	int cantidadPaginasPorSegmento; /*Páginas que ocupará cada proceso*/
	int cantidadSegundos;  /*Cantidad de segundos que durará el proceso*/
	int cantidadSegundosPorProceso;  /*Cada cuántos segundos se hará un proceso*/
	int numeroProcesos;
	int cantidadTotalPaginas;
	
	char *mensaje;

	ProcesoSegmentacion *proceso;

  
	hora = time(NULL);  
    // Semilla de rand();
    srand(hora);

    
   
    numeroProcesos = 0;

    /*La idea, por ahora, es crear 1000 procesos que pidan memoria en la compartida*/
    while(numeroProcesos < 1000){
    	//31 + 30
    	
    	mensaje = (char*)malloc(sizeof(char*));



	
    	cantidadSegundosPorProceso = rand()% 30 + 30;  /*Random entre 30 y 60*/ 
    	cantidadSegmentos = rand()% 5 + 1;   /*Random entre 1 y 5*/
    	cantidadSegundos = rand()% 40 + 20; /*Random entre 20 y 60*/    	
    	

    	cantidadSegmentos = 3;   /*Random entre 1 y 5*/
    	cantidadSegundos = rand()% 40 + 20; /*Random entre 20 y 60*/    	
    	

    	proceso->idProceso = numeroProcesos;
    	proceso->tiempo = cantidadSegundos;
    	proceso->numeroSegmentos = cantidadSegmentos;


    	cantidadTotalPaginas = 0;


    	//Para enseñar en pruebas

    	/*
    	
    	proceso->segmentos[0].numeroPaginas = 3;
    	proceso->segmentos[0].numeroSegmento = 0;

    	proceso->segmentos[1].numeroPaginas = 4;
    	proceso->segmentos[1].numeroSegmento = 1;

    	*/


    	/*
    	proceso->segmentos[0].numeroPaginas = 4;
    	proceso->segmentos[0].numeroSegmento = 0;

    	proceso->segmentos[1].numeroPaginas = 3;
    	proceso->segmentos[1].numeroSegmento = 1;

    	proceso->segmentos[2].numeroPaginas = 5;
    	proceso->segmentos[2].numeroSegmento = 2;
    	*/

    	/*
    	proceso->segmentos[0].numeroPaginas = 3;
    	proceso->segmentos[0].numeroSegmento = 0;

    	proceso->segmentos[1].numeroPaginas = 3;
    	proceso->segmentos[1].numeroSegmento = 1;

    	proceso->segmentos[2].numeroPaginas = 3;
    	proceso->segmentos[2].numeroSegmento = 2;
    	*/
    	
    	
    	for(int i  = 0; i<cantidadSegmentos; i++){

    		cantidadPaginasPorSegmento = rand()% 5 + 1; /*Random entre 1 y 3*/    		
    		proceso->segmentos[i].numeroSegmento = i;
    		proceso->segmentos[i].numeroPaginas = cantidadPaginasPorSegmento;

    		cantidadTotalPaginas+=cantidadPaginasPorSegmento;
    	}

    	
	
    	
    	printf("--- ¡Se va a generar un nuevo proceso! ---\n");

    	printf("-------------------------------------------------\n");
    	printf("Segundos para el proceso: %i\n", cantidadSegundos);	
    	printf("Segmentos: \n");
    	printf("-----------------\n");
    	printf("\n");

    	sprintf(mensaje, "--- ¡Se va a generar un nuevo proceso! ---\n");
    	sprintf(mensaje +strlen(mensaje), "-------------------------------------------------\n");
    	sprintf(mensaje +strlen(mensaje), "-------------------------------------------------\n");
    	sprintf(mensaje +strlen(mensaje), "Segundos para el proceso: ");
    	
    	sprintf(mensaje +strlen(mensaje), "%d", cantidadSegundos);
    	
    	
    	sprintf(mensaje +strlen(mensaje), "\n");
    	sprintf(mensaje +strlen(mensaje), "Segmentos: \n");
    	sprintf(mensaje +strlen(mensaje), "-----------------\n\n");

    	for (int i = 0; i<cantidadSegmentos; i++){

    		int nSegmento; //Meterlo en enteros porque con punteros se cae D:
    		int nPaginas;

    		nSegmento = proceso->segmentos[i].numeroSegmento;
    		nPaginas = proceso->segmentos[i].numeroPaginas;
    		printf("--------\n");
    		printf("Segmento: %i\n", proceso->segmentos[i].numeroSegmento);
    		printf("Páginas para el segmento: %i\n", proceso->segmentos[i].numeroPaginas);
    		printf("--------\n");

    		sprintf(mensaje +strlen(mensaje), "--------\n");
    		sprintf(mensaje +strlen(mensaje), "Segmento: ");
    		sprintf(mensaje +strlen(mensaje), "%d", nSegmento);
    		sprintf(mensaje +strlen(mensaje), "\n");
    		sprintf(mensaje +strlen(mensaje), "Páginas para el segmento: ");
    		sprintf(mensaje +strlen(mensaje), "%d", nPaginas);
    		sprintf(mensaje +strlen(mensaje), "\n");
    		sprintf(mensaje +strlen(mensaje), "--------\n");
    		

    	}
    	printf("-----------------\n");
    	printf("\n");
    	printf("Cantidad de páginas total: %i\n", cantidadTotalPaginas);	
    	printf("Tiempo hasta el próximo proceso: %i\n", cantidadSegundosPorProceso);
    	printf("-------------------------------------------------\n");
    	printf("\n\n");

    	sprintf(mensaje +strlen(mensaje), "-----------------\n\n");
    	sprintf(mensaje +strlen(mensaje), "Cantidad de páginas total: ");
    	sprintf(mensaje +strlen(mensaje), "%d", cantidadTotalPaginas);
    	sprintf(mensaje +strlen(mensaje), "\n");
    	sprintf(mensaje +strlen(mensaje), "Tiempo hasta el próximo proceso: ");
    	sprintf(mensaje +strlen(mensaje), "%d", cantidadSegundosPorProceso);
    	sprintf(mensaje +strlen(mensaje), "\n");
    	sprintf(mensaje +strlen(mensaje), "-------------------------------------------------\n\n\n");

    	
    	escribirEnBitacora(mensaje);

    	numeroProcesos++;

    	crearHiloSegmentacion(proceso);
    	//sleep(cantidadSegundosPorProceso);
    	sleep(2);

    	//

    }
}

/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------------ */


/*--------------------------------------  Acceso a la memoria compartida  --------------------------------------*/


/* ------------------------------------------------------------------------------------------------ */


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
   
		printf("** No se pudo abrir el a<<rchivo correctamente **\n");

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
int leerMemoria(int idMemoriaCompartida, int idMemoriaCompartidaBitacora, int memoria, int pide, int bloqueado, int muerto, int terminado, int pId){

	key_t key; /*Clave que se pasa al shmget*/
	int banderaMemoriaCompartida; /*Necesaria para saber bajo qué "modalidad" se crea la memoria compartida*/
	int tamanioMemoriaCompartida;   /*Tamaño que se le pedirá al sistema operativo para la memoria compartida*/ 
	char *memoriaCompartida; /*Con esta variable se puede acceder al contenido de la memoria compartida*/
	
	//key = 5678;
	
	
	/*Se valida si se logra o no compartir la memoria*/
	//if(idMemoriaCompartida < 0){
		
	//	printf("** Error, no se pudo asignar la memoria compartida **\n");
	//	return -1;

	//}

	paginas = shmat(idMemoriaCompartida, NULL, 0); /*Revisar*/
	contenidoBitacora = shmat(idMemoriaCompartidaBitacora, NULL, 0);
	procesosEnMemoria = shmat(memoria, NULL, 0);
	procesoPideMemoria = shmat(pide, NULL, 0);
	procesosBloqueados = shmat(bloqueado, NULL, 0);
	procesosMuertos = shmat(muerto, NULL, 0);
	procesosTerminados = shmat(terminado, NULL, 0);

	pID = shmat(pId, NULL, 0);
	pID[0] = getpid();

	printf("PID: %i\n", pID[0]);

	/*Se valida si se obtuvo acceso a la memoria compartida correctamente*/
	if(contenidoBitacora == (char *) -1){

		printf("-- Error al conectarse con la bitácora --");
		return -1;

	}

	
	    	
	
	return 1;

}

/* ------------------------------------------------------------------------------------------------ */






/* Desde esta función se llamará a las funciones requeridas para cumplir con la funcionalidad de este programa */
int main(){
	
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



		leerMemoria(idMemoriaCompartida, idMemoriaCompartidaBitacora, memoria, pide, bloqueado, muerto, terminado, idPid);

		char *opcion; /*Aquí se almacenará el valor de entrada de la consola*/
	
		//return 0;

		int opcionN;  /*Aquí se almacenará el valor entero de la entrada a la consola, para el switch*/

		opcion = (char*)malloc(sizeof(char*));
		//contenidoBitacora =  (char*)malloc(sizeof(char*));
		//contenidoEspia =  (char*)malloc(sizeof(char*));

		printf("MI ID: %i", getpid());


		printf("Bienvenido(a), este es el programa que se encargará de producir procesos (Threads) por favor, seleccione un modelo:\n");
		printf("****************************\n");
		printf("***    1-Paginación      ***\n");
		printf("***    2-Segmentación    ***\n");
		printf("****************************\n");
		scanf("%s", opcion);


	
		opcionN = atoi(opcion); /*Conversión a tipo entero de la entrada en consola*/

		/*Control principal de las opciones, por este medio se llamará al modelo de paginación o segmentación*/
		switch(opcionN){

			case 1:
				printf("** Ha seleccionado el mecanismo de paginación **\n");
				mecanismoPaginacion();
				break;

			case 2:
				printf("** Ha seleccionado el mecanismo de segmentación **\n");
				mecanismoSegmentacion();
				break;

			default:
				printf("No ha digitado una opción válida\n");	


		}
	}

	
	



}