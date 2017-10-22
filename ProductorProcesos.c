/* Este programa estará encargado de pedir la memoria compartida
   incluyendo las páginas que requerirá para los procesos */


#include <sys/types.h> /*Manejo de memoria compartida*/
#include <sys/ipc.h>   /*Manejo de memoria compartida*/
#include <sys/shm.h>   /*Manejo de memoria compartida*/
#include <stdio.h> 
#include <pthread.h>   /*Para el manejo de  hilos*/
#include <time.h>
#include <stdlib.h>    /*Para el uso de rand()*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "Pagina.h"
#include "Configuraciones.h"
#include "ProcesoPaginacion.h"
#include "ProcesoSegmentacion.h"


#define tamanioPasoArchivo 100 /*Tamaño máximo para pasar el contenido del txt a una variable*/





int semaforo;
Pagina *paginas;

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
	
	contador = 0;

	printf("-----------------------------------------------------------------------\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");

	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado);
		
		/*Se pregunta si hay espacio disponible, en caso de que sí, se suma el contador para saber cuántas páginas hay*/
		if(paginas[i].disponible == 1) contador++;

	}

	printf("--- Páginas disponibles: %i ---\n", contador);
	
	if(contador >= numeroPaginas){

		printf("--- ¡Hay suficientes páginas para ejecutar este proceso! :) --- \n");
		printf("-----------------------------------------------------------------------\n");
		
		return 1; /*Hay suficientes páginas*/

	} 
	
	else{

		printf("--- ¡No hay suficientes páginas para ejecutar este proceso! :( --- \n");
		printf("-----------------------------------------------------------------------\n");
		return 0; /*No hay suficientes páginas*/
		
	} 


}

/*Función encargada de ocupar las páginas con procesos, en el mecanismo de paginación*/
void tomarMemoriaPaginacion(int numeroPaginas, int idProceso){

	int contador; /*Para delimitar cuando se ocupen las páginas que se requieren*/

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

	printf("--- Se asignaron las páginas para el proceso: %i ---\n", idProceso);
	printf("--- Así se ve la memoria ahora: ---");

	printf("-----------------------------------------------------------------------\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");

	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado);
		
	}

	printf("-----------------------------------------------------------------------\n");


} 

/* Función encargada de devolver el espacio en la memoria compartida para el proceso en el mecanismo de paginación */
void liberarMemoriaPaginacion(int idProceso){

	printf("--- Se liberarán las páginas para el proceso: %i ---\n", idProceso);
	

	for(int i = 0; i<tamanio; i++){

		/*Pregunta si un campo ocupado coincide con el id que se desea sacar*/
		if(paginas[i].procesoOcupado == idProceso && paginas[i].disponible == 0){ 

			paginas[i].procesoOcupado = -1; //No le queda ningún proceso asignado
			paginas[i].disponible = 1; //Vuelve a estar disponible

		}

	}

	printf("--- Se han liberado las páginas para el proceso: %i ---\n", idProceso);
	

}

/*Función encargada de ejecutar el proceso en el modelo de paginación, tiene como parámetro el proceso, con las páginas que ocupa y el tiempo que se ejecutará*/
int *ejecucionProcesoPaginacion(void *proceso){
	/*FALTA ESCRIBIR EN LA BITÁCORA*/	

	int aceptaSolicitudMemoria; /*Para saber si hay espacio suficiente en memoria*/
	int idProceso;
	int cantidadPaginas;

	ProcesoPaginacion *procesoP;

	procesoP = (ProcesoPaginacion*) proceso; /*Cast*/
	idProceso = procesoP->idProceso;
	cantidadPaginas = procesoP->cantidadPaginas;

	printf("--------------------- Proceso: %i --------------------- \n", procesoP->idProceso);

	printf("----------------------------------------------------------------------------------------------------------------------\n");
	
	printf("------------ Esperando para usar el semáforo ------------ \n");
	
	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	printf("------------ Entra al semaforo               ------------\n");

	printf("------------ Páginas requeridas: %i          ------------\n", cantidadPaginas);


	aceptaSolicitudMemoria = solicitarMemoriaPaginacion(procesoP->cantidadPaginas);
	
	/*Si no hay suficiente espacio, se mata el proceso y se devuelve el semáforo*/
	if(aceptaSolicitudMemoria == 0){

		printf("Matando al proceso\n");
		printf("----------------------------------------------------------------------------------------------------------------------\n");
		doSignal(semaforo,0); /*Libera el semáforo*/

		return -1;	
	}

	/*Se ocupan los campos en memoria*/
	tomarMemoriaPaginacion(procesoP->cantidadPaginas, procesoP->idProceso);

	doSignal(semaforo,0); /*Libera semáforo*/
	
	/*Escribir en bitácora*/
	printf("----- Comienza el sleep del proceso: %i -----\n", idProceso);
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	

	sleep(6);

	printf("----- Termina el sleep del proceso: %i -----\n", idProceso);
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	

	//sleep(procesoP->tiempo);

	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	liberarMemoriaPaginacion(idProceso);	

	doSignal(semaforo,0); /*Libera el semáforo*/

	

	printf("--- Terminó la ejecución de este proceso ---\n");
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
	
	contador = 0;
	marca = 0;

	printf("--- Buscando espacios contiguos para el proceso: %i segmento: %i páginas requeridas: %i ---\n", idProceso, segmento, numeroPaginas);
	printf("-----------------------------------------------------------------------------------------\n");
	printf("--- Así se ve la memoria ahora ---\n");
	printf("-----------------------------------------------------------------------\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");

	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i Segmento: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado, paginas[i].numeroSegmento);
		
	}
	printf("-----------------------------------------------------------------------\n");
	
	
	for(int i = 0; i<tamanio; i++){
		
		/*Se pregunta si hay espacio disponible, en caso de que sí, se suma el contador para saber cuántas páginas hay*/
		if(paginas[i].disponible == 1){ 
			
			contador++;
		
		}

		else{
			
			if(contador >= numeroPaginas){
				printf("--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: %i para el segmento: %i comienza en la celda: %i! :) ---\n", numeroPaginas, segmento, marca);
				printf("-----------------------------------------------------------------------------------------\n");
	
				return marca; /*Retorna el índice de la página donde empiezan contiguas*/

			}

			else{
				
				marca = i+1;
				contador = 0;	
			
			} 
		
		}

	}

	if(contador >= numeroPaginas){
		printf("--- ¡Se logró encontrar un espacio con páginas contiguas de tamaño: %i para el segmento: %i comienza en la celda: %i! :) ---\n", numeroPaginas, segmento, marca);
		printf("-----------------------------------------------------------------------------------------\n");
	
		return marca; /*Retorna el índice de la página donde empiezan contiguas*/

	}

	else{

		printf("--- ¡No se logró encontrar un espacio con páginas contiguas de tamaño: %i para el segmento: %i :(! ---\n", numeroPaginas, segmento);
		printf("-----------------------------------------------------------------------------------------\n");
		return -1;	
	
	} 


}

int asignarContiguas(int idProceso, int cantidadPaginas, int marca, int numeroSegmento){

	
	int marcaAux;
	marcaAux = marca;
	
	printf("--- Buscando espacios contiguos para el proceso: %i segmento: %i páginas requeridas: %i ---\n", idProceso, numeroSegmento, cantidadPaginas);
	
	printf("--------------------------------------------------------------------------------------------------------\n");
	for(int i = 0; i<cantidadPaginas; i++){
		
		paginas[marcaAux].procesoOcupado = idProceso;
		paginas[marcaAux].disponible = 0;
		paginas[marcaAux].numeroSegmento = numeroSegmento;
		//paginas[marcaAux].numeroSegmento = numeroSegmento;
		marcaAux++;

	}

	printf("--- Así se ve la memoria después de la asignación ---\n");
	printf("-----------------------------------------------------------------------\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");

	for(int i = 0; i<tamanio; i++){
		printf("Página %i: Disponible: %i Número de proceso: %i Segmento: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado, paginas[i].numeroSegmento);
	}
	printf("-----------------------------------------------------------------------\n");
	printf("--------------------------------------------------------------------------------------------------------\n");
	
	return 0;


}


/* Función encargada de solicitar espacio en la memoria compartida para el proceso en el mecanismo de segmentación */
int solicitarMemoriaSegmentacion(ProcesoSegmentacion *proceso){

	int paginasTotalesProceso;
	paginasTotalesProceso = calcularTotalPaginasProceso(proceso);

	int contador; /*Para saber cuántos espacios libres hay*/

	
	contador = 0;

	printf("------------ Páginas requeridas: %i          ------------\n", paginasTotalesProceso);
	

	printf("-----------------------------------------------------------------------\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");

	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i Segmento: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado, paginas[i].numeroSegmento);
		
		/*Se pregunta si hay espacio disponible, en caso de que sí, se suma el contador para saber cuántas páginas hay*/
		if(paginas[i].disponible == 1) contador++;

	}

	printf("--- Páginas disponibles: %i ---\n", contador);
	if(contador >= paginasTotalesProceso){

		printf("--- ¡Hay suficientes páginas para ejecutar este proceso! :) --- \n");
		printf("-----------------------------------------------------------------------\n"); 
		return 1; /*Hay suficientes páginas*/
	}
	
	else{
		printf("--- ¡No hay suficientes páginas para ejecutar este proceso! :( --- \n");
		printf("-----------------------------------------------------------------------\n"); 
		return 0; /*No hay suficientes páginas*/
	}

}

/* Función encargada de devolver el espacio en la memoria compartida para el proceso en el mecanismo de segmentación */
void liberarMemoriaSegmentacion(int idProceso){

	for(int i = 0; i<tamanio; i++){

		/*Pregunta si un campo ocupado coincide con el id que se desea sacar*/
		if(paginas[i].procesoOcupado == idProceso && paginas[i].disponible == 0){ 

			paginas[i].procesoOcupado = -1; //No le queda ningún proceso asignado
			paginas[i].disponible = 1; //Vuelve a estar disponible
			paginas[i].numeroSegmento = -1;


		}

	}
	
}





/*Función encargada de ejecutar el proceso en el modelo de segmentación*/
int *ejecucionProcesoSegmentacion(void *proceso){

	/*FALTA ESCRIBIR EN LA BITÁCORA*/	

	int aceptaSolicitudMemoria;
	int idProceso;
	int tiempo;
	int numeroSegmentos;
	int cantidadPaginas;

	ProcesoSegmentacion *procesoS;

	procesoS =  (ProcesoSegmentacion *) proceso; /*Cast*/
	idProceso = procesoS->idProceso;
	tiempo = procesoS->tiempo;
	numeroSegmentos = procesoS->numeroSegmentos;  

	
	printf("--------------------- Proceso: %i --------------------- \n", idProceso);

	printf("----------------------------------------------------------------------------------------------------------------------\n");
	
	printf("------------ Esperando para usar el semáforo ------------ \n");
	
	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	printf("------------ Entra al semaforo               ------------\n");

	
	aceptaSolicitudMemoria = solicitarMemoriaSegmentacion(procesoS);
	
	if(aceptaSolicitudMemoria == 0){

		printf("--- Matando al proceso ---\n");
		printf("----------------------------------------------------------------------------------------------------------------------\n");
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
			
			printf("--- Matando al proceso ---\n");

			liberarMemoriaSegmentacion(idProceso);
			printf("----------------------------------------------------------------------------------------------------------------------\n");
			doSignal(semaforo,0); /*Libera el semáforo*/

			return -1;
		}

		else asignarContiguas(idProceso, numeroPaginas, marca, numeroSegmento);

	}

	//ESCRIBIR EN BITÁCORA



	doSignal(semaforo,0);
	
	printf("----- Comienza el sleep del proceso: %i -----\n", idProceso);
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	


	sleep(5);
	//sleep(tiempo);
	
	
	printf("----- Termina el sleep del proceso: %i -----\n", idProceso);
	printf("----------------------------------------------------------------------------------------------------------------------\n");
	

	
	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	liberarMemoriaSegmentacion(idProceso);

	//ESCRIBIR EN BITÁCORA
	
	doSignal(semaforo,0); /*Libera el semáforo*/
			

	printf("Terminó la ejecución de este proceso\n");
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
	ProcesoSegmentacion *proceso;

  
	hora = time(NULL);  
    // Semilla de rand();
    srand(hora);

    
   
    numeroProcesos = 0;

    /*La idea, por ahora, es crear 1000 procesos que pidan memoria en la compartida*/
    while(numeroProcesos < 1000){
    	//31 + 30
    	
    	cantidadSegundosPorProceso = rand()% 30 + 30;  /*Random entre 30 y 60*/ 
    	cantidadSegmentos = rand()% 5 + 1;   /*Random entre 1 y 5*/
    	cantidadSegundos = rand()% 40 + 20; /*Random entre 20 y 60*/    	
    	proceso->idProceso = numeroProcesos;
    	proceso->tiempo = cantidadSegundos;
    	proceso->numeroSegmentos = cantidadSegmentos;

    	cantidadTotalPaginas = 0;

    	
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
    	for (int i = 0; i<cantidadSegmentos; i++){

    		printf("--------\n");
    		printf("Segmento: %i\n", proceso->segmentos[i].numeroSegmento);
    		printf("Páginas para el segmento: %i\n", proceso->segmentos[i].numeroPaginas);
    		printf("--------\n");
    		

    	}
    	printf("-----------------\n");
    	printf("Cantidad de páginas total: %i\n", cantidadTotalPaginas);	
    	printf("Tiempo hasta el próximo proceso: %i\n", cantidadSegundosPorProceso);
    	printf("-------------------------------------------------\n");
    	
    	

    	numeroProcesos++;

    	crearHiloSegmentacion(proceso);
    	//sleep(cantidadSegundosPorProceso);
    	sleep(2);

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

	paginas = shmat(idMemoriaCompartida, NULL, 0); /*Revisar*/
	//memoriaCompartida = shmat(idMemoriaCompartida, NULL, 0); /*Se accede a la memoria compartida*/


	/*Se valida si se obtuvo acceso a la memoria compartida correctamente*/
	if(memoriaCompartida == (char *) -1){

		printf("Error");
		return -1;

	}
	    
	/*Hacerlo dinámico*/
	//for(int i = 0; i<15; i++){

	//	printf("Disponibilidad: %i\n", paginas[i].disponible);
	//	printf("Proceso ocupado: %i\n", paginas[i].procesoOcupado);

	//}
	
	
	return 1;

}

/* ------------------------------------------------------------------------------------------------ */






/* Desde esta función se llamará a las funciones requeridas para cumplir con la funcionalidad de este programa */
int main(){
	
	int idMemoriaCompartida;
	idMemoriaCompartida = leerIdMemoriaCompartida();
	leerMemoria(idMemoriaCompartida);

	char *opcion; /*Aquí se almacenará el valor de entrada de la consola*/
	
	//return 0;

	int opcionN;  /*Aquí se almacenará el valor entero de la entrada a la consola, para el switch*/

	opcion = (char*)malloc(sizeof(char*));

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