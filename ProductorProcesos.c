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

/*Esto es solo para probar si los semáforos sirven*/

/* ********************************************************  */

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

/* ********************************************************  */


/* Función encargada de solicitar espacio en la memoria compartida para el proceso en el mecanismo de paginación */
int solicitarMemoriaPaginacion(int numeroPaginas){
	
	int contador; /*Para saber cuántos espacios libres hay*/

	
	contador = 0;

	printf("*************************************************\n");
	printf("Los que tienen -1 y 1, son espacios libres\n");

	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado);
		
		/*Se pregunta si hay espacio disponible, en caso de que sí, se suma el contador para saber cuántas páginas hay*/
		if(paginas[i].disponible == 1) contador++;

	}

	printf("Páginas disponibles: %i\n", contador);
	printf("*************************************************\n");
	
	if(contador >= numeroPaginas) return 1; /*Hay suficientes páginas*/
	
	else return 0; /*No hay suficientes páginas*/


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

} 

/* Función encargada de devolver el espacio en la memoria compartida para el proceso en el mecanismo de paginación */
void liberarMemoriaPaginacion(int idProceso){

	for(int i = 0; i<tamanio; i++){

		/*Pregunta si un campo ocupado coincide con el id que se desea sacar*/
		if(paginas[i].procesoOcupado == idProceso && paginas[i].disponible == 0){ 

			paginas[i].procesoOcupado = -1; //No le queda ningún proceso asignado
			paginas[i].disponible = 1; //Vuelve a estar disponible


		}

	}


}


/*Función encargada de calcular el número de páginas totales requeridas para un proceso en el mecanismo de segmentación*/
int calcularTotalPaginasProceso(ProcesoSegmentacion *proceso){

	int numeroSegmentos = proceso->numeroSegmentos;
	int paginasTotalesProceso;

	paginasTotalesProceso = 0;

	printf("NumeroSegmentos: %i\n", proceso->numeroSegmentos);

	for(int i = 0; i<numeroSegmentos; i++){

		paginasTotalesProceso+=proceso->segmentos[i].numeroPaginas;


	}

	printf("Total de páginas para el proceso: %i\n", paginasTotalesProceso);
	
	return paginasTotalesProceso;

}


/* Función encargada de solicitar espacio en la memoria compartida para el proceso en el mecanismo de segmentación */
int solicitarMemoriaSegmentacion(ProcesoSegmentacion *proceso){

	int paginasTotalesProceso;
	paginasTotalesProceso = calcularTotalPaginasProceso(proceso);

	int contador; /*Para saber cuántos espacios libres hay*/

	
	contador = 0;

	//printf("*************************************************\n");
	//printf("Los que tienen -1 y 1, son espacios libres\n");

	for(int i = 0; i<tamanio; i++){
		
		//printf("Página %i: Disponible: %i Número de proceso: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado);
		
		/*Se pregunta si hay espacio disponible, en caso de que sí, se suma el contador para saber cuántas páginas hay*/
		if(paginas[i].disponible == 1) contador++;

	}

	printf("Páginas disponibles: %i\n", contador);
	//printf("*************************************************\n");
	
	if(contador >= paginasTotalesProceso) return 1; /*Hay suficientes páginas*/
	
	else return 0; /*No hay suficientes páginas*/

}

/* Función encargada de devolver el espacio en la memoria compartida para el proceso en el mecanismo de segmentación */
void liberarMemoriaSegmentacion(int idProceso){

	for(int i = 0; i<tamanio; i++){

		/*Pregunta si un campo ocupado coincide con el id que se desea sacar*/
		if(paginas[i].procesoOcupado == idProceso && paginas[i].disponible == 0){ 

			paginas[i].procesoOcupado = -1; //No le queda ningún proceso asignado
			paginas[i].disponible = 1; //Vuelve a estar disponible


		}

	}
	
}

/*Función encargada de ejecutar el proceso en el modelo de paginación, tiene como parámetro el proceso, con las páginas que ocupa y el tiempo que se ejecutará*/
int *ejecucionProcesoPaginacion(void *proceso){
	/*FALTA ESCRIBIR EN LA BITÁCORA*/	

	int aceptaSolicitudMemoria; /*Para saber si hay espacio suficiente en memoria*/

	ProcesoPaginacion *procesoP;

	procesoP = (ProcesoPaginacion*) proceso; /*Cast*/

		
	
	printf("Esperando para usar el semáforo \n");
	
	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	printf("Entra al semaforo\n");

	aceptaSolicitudMemoria = solicitarMemoriaPaginacion(procesoP->cantidadPaginas);
	
	/*Si no hay suficiente espacio, se mata el proceso y se devuelve el semáforo*/
	if(aceptaSolicitudMemoria == 0){

		printf("Matando al proceso\n");
		doSignal(semaforo,0); /*Libera el semáforo*/

		return -1;	
	}

	/*Se ocupan los campos en memoria*/
	tomarMemoriaPaginacion(procesoP->cantidadPaginas, procesoP->idProceso);

	doSignal(semaforo,0); /*Libera semáforo*/
	
	/*Escribir en bitácora*/

	sleep(15);  
	//sleep(procesoP.tiempo);

	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	liberarMemoriaPaginacion(procesoP->idProceso);	

	doSignal(semaforo,0); /*Libera el semáforo*/

	

	printf("Terminó la ejecución de este proceso\n");
	return 1;
	/*Aparentemente no se necesita ninguna línea de código para liberar al hilo, jejeps*/

}

int apartarPaginasContiguas(int numeroPaginas){


	int contador; /*Para saber cuántos espacios libres hay*/

	int marca; /*Para saber dónde empieza una serie de bloque contiguos*/
	
	contador = 0;
	marca = 0;

	
	for(int i = 0; i<tamanio; i++){
		
		/*Se pregunta si hay espacio disponible, en caso de que sí, se suma el contador para saber cuántas páginas hay*/
		if(paginas[i].disponible == 1){ 
			contador++;
		}

		else{
			
			if(contador >= numeroPaginas){

				return marca; /*Retorna el índice de la página donde empiezan contiguas*/

			}

			else{
				marca = i+1;
				contador = 0;	
			} 
		
		}

	}


	if(contador >= numeroPaginas){

		return marca; /*Retorna el índice de la página donde empiezan contiguas*/

	}

	else{
		return -1;	
	} 


}

void asignarContiguas(int idProceso, int cantidadPaginas, int marca, int numeroSegmento){

	
	int marcaAux;
	marcaAux = marca;
	
	for(int i = 0; i<cantidadPaginas; i++){
		
		paginas[marcaAux].procesoOcupado = idProceso;
		paginas[marcaAux].disponible = 0;
		paginas[marcaAux].numeroSegmento = numeroSegmento;
		//paginas[marcaAux].numeroSegmento = numeroSegmento;
		marcaAux++;

	}

	for(int i = 0; i<tamanio; i++){
		printf("Página %i: Disponible: %i Número de proceso: %i Segmento: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado, paginas[i].numeroSegmento);
	}


	


}

/*Función encargada de ejecutar el proceso en el modelo de segmentación*/
int *ejecucionProcesoSegmentacion(void *proceso){

	/*FALTA ESCRIBIR EN LA BITÁCORA*/	

	int aceptaSolicitudMemoria;
	int numeroSegmentos;
	int idProceso;

	ProcesoSegmentacion *procesoS;

	procesoS =  (ProcesoSegmentacion *) proceso; /*Cast*/
	numeroSegmentos = procesoS->numeroSegmentos;
	idProceso = procesoS->idProceso;

	printf("-----------------------------------\n");

	for (int i = 0; i<procesoS->numeroSegmentos; i++){

    	printf("SEGMENTO: %i PAGINAS: %i\n", i, procesoS->segmentos[i].numeroPaginas);
    }

    //sleep(1010291092);

	printf("EMTRA\n");
	printf("*********************************\n");
    printf("Proceso: %i\n", procesoS->idProceso);
    printf("Tiempo: %i\n", procesoS->tiempo);
    	

	
	printf("Esperando para usar el semáforo \n");
	
	doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	printf("Entra al semaforo\n");

	aceptaSolicitudMemoria = solicitarMemoriaSegmentacion(procesoS);
	
	if(aceptaSolicitudMemoria == 0){

		printf("Matando al proceso\n");
		doSignal(semaforo,0); /*Libera el semáforo*/

		return -1;	
	}


	for(int i = 0; i<procesoS->numeroSegmentos; i++){

		int numeroPaginas;
		int marca;
		int numeroSegmento;

		printf("PAGINITAS: %i\n", procesoS->segmentos[i].numeroPaginas);

		numeroPaginas = procesoS->segmentos[i].numeroPaginas;
		numeroSegmento = procesoS->segmentos[i].numeroSegmento;

		marca = apartarPaginasContiguas(numeroPaginas);

		if(marca == -1){
			
			printf("Matando al proceso\n");
			liberarMemoriaSegmentacion(idProceso);
			doSignal(semaforo,0); /*Libera el semáforo*/

			return -1;
		}

		else asignarContiguas(idProceso, numeroPaginas, marca, numeroSegmento);
		//sleep(2);
		
		//printf("AHASHAHSA: %i", n);


	}
	doSignal(semaforo,0);
	

	sleep(5);
	//tomarMemoriaPaginacion(procesoP.cantidadPaginas, procesoP.idProceso);

	
	/*Escribir en bitácora*/

	//sleep(10);  
	//sleep(procesoP.tiempo);

	//doWait(semaforo,0); /*Solicita el semáforo, si está siendo utilizado, el proceso queda en espera*/
	
	//liberarMemoriaPaginacion(procesoP.idProceso);	

	//doSignal(semaforo,0); /*Libera el semáforo*/

	liberarMemoriaSegmentacion(idProceso);
			

	printf("Terminó la ejecución de este proceso\n");
	return 1;
	/*Aparentemente no se necesita ninguna línea de código para liberar al hilo, jejeps*/


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
							
	
	//proceso->idProceso = pthread_create( &sniffer_thread , &tattr ,  &ejecucionProcesoPaginacion ,  proceso); 
	idCreacionHilo = pthread_create( &sniffer_thread , &tattr ,  &ejecucionProcesoPaginacion ,  proceso);	
	

	printf("SE CREO HILO: %i\n", idCreacionHilo);						 
	if( proceso->idProceso < 0)
	{
		perror("No se pudo crear el hilo correctamente");
		
	}

}

/*Función encargada de crear procesos para el mecanismo de segmentación*/
void crearHiloSegmentacion(ProcesoSegmentacion *proceso){

	int ret;
	int idCreacionHilo;
	pthread_attr_t tattr;
	pthread_t sniffer_thread;

	printf("*********************************\n");
    printf("Proceso: %i\n", proceso->idProceso);
    printf("Tiempo: %i\n", proceso->tiempo);
    
    for (int i = 0; i<proceso->numeroSegmentos; i++){

    	printf("SEGMENTO: %i PAGINAS: %i\n", i, proceso->segmentos[i].numeroPaginas);
    }


	
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

    	printf("*********************************\n");
    	printf("Proceso: %i\n", numeroProcesos);
    	printf("Páginas para el proceso: %i\n", cantidadPaginas);
    	printf("Segundos para el proceso: %i\n", cantidadSegundos);	
    	printf("*********************************\n");

    	proceso->idProceso = numeroProcesos;
    	proceso->cantidadPaginas = cantidadPaginas;
    	proceso->tiempo = cantidadSegundos;


    	numeroProcesos++;

    	printf("Tiempo hasta el próximo proceso: %i\n", cantidadSegundosPorProceso);
    	crearHiloPaginacion(proceso);
    	//sleep(cantidadSegundosPorProceso);
    	sleep(2);





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


    	//printf("Cantidad segmentos por asignar: %i\n", cantidadSegmentos);
    	//printf("Cantidad de segundos por asignar: %i\n", cantidadSegundos);

    	proceso->idProceso = numeroProcesos;
    	proceso->tiempo = cantidadSegundos;
    	proceso->numeroSegmentos = cantidadSegmentos;

    	printf("SEGMENTITOS: %i\n", proceso->numeroSegmentos);

    	for(int i  = 0; i<cantidadSegmentos; i++){

    		cantidadPaginasPorSegmento = rand()% 5 + 1; /*Random entre 1 y 3*/

    		printf("Cantidad de páginas por asignar para el segmento: %i\n", cantidadPaginasPorSegmento);

    		
    		proceso->segmentos[i].numeroSegmento = i;
    		proceso->segmentos[i].numeroPaginas = cantidadPaginasPorSegmento;

    		
    	}

    	
    	
    	printf("*********************************\n");
    	printf("Proceso: %i\n", proceso->idProceso);
    	printf("Tiempo: %i\n", proceso->tiempo);
    	printf("Segmentos: \n");
    	for (int i = 0; i<cantidadSegmentos; i++){

    		printf("***********\n");
    		printf("Segmento: %i\n", proceso->segmentos[i].numeroSegmento);
    		printf("Páginas para el segmento: %i\n", proceso->segmentos[i].numeroPaginas);
    		printf("***********\n");
    		

    	}
    	printf("*********************************\n");

    	

    	numeroProcesos++;

    	printf("Tiempo hasta el próximo proceso: %i\n", cantidadSegundosPorProceso);
    	crearHiloSegmentacion(proceso);
    	//sleep(cantidadSegundosPorProceso);
    	sleep(2);





    }

}



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