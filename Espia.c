/* Este programa estará encargado de dar información sobre
   los procesos que se encuentran en el productor de procesos */

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



#define tamanioPasoArchivo 100 /*Tamaño máximo para pasar el contenido del txt a una variable*/

Pagina *paginas;
ProcesoGeneral *procesosEnMemoria;
ProcesoGeneral *procesoPideMemoria;
ProcesoGeneral *procesosBloqueados;
ProcesoGeneral *procesosMuertos;
ProcesoGeneral *procesosTerminados;

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

/*Función encargada de mostrar la memoria principal, junto con sus páginas*/
void verEstadoMemoria(){

	int idMemoriaCompartida;
	idMemoriaCompartida = leerIdMemoriaCompartida("idMemoriaCompartida.txt");
	paginas = shmat(idMemoriaCompartida, NULL, 0); /*Revisar*/
	
	printf("--- Viendo el estado de la memoria actual ---\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	printf("-----------------------------------------------------------------\n");

	printf("\n\n");

	/*Se recorren las páginas*/
	for(int i = 0; i<tamanio; i++){
		
		printf("Página %i: Disponible: %i Número de proceso: %i\n", paginas[i].numeroPagina, paginas[i].disponible, paginas[i].procesoOcupado);
		
	}
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	
}

/*Función encargada de mostrar los procesos actuales en memoria*/
void verProcesosEnMemoria(){

	int contador;
	int idProcesosEnMemoria;
	idProcesosEnMemoria = leerIdMemoriaCompartida("procesosMemoria.txt");
	procesosEnMemoria = shmat(idProcesosEnMemoria, NULL, 0);

	contador = 0;

	printf("--- Viendo los procesos en memoria en el momento ---\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	
	/*Se recorren los procesos en memoria*/
	for(int i = 0; i<tamanioEspiaProcesosMemoria; i++){

		if(procesosEnMemoria[i].idProceso != -1){
			
			printf("Proceso: %i\n", procesosEnMemoria[i].idProceso);
			printf("Tipo de mecanismo: %s\n", procesosEnMemoria[i].tipoMecanismo);
			contador++;


		}

	}
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	

	if(contador == 0){

		printf("--- No hay procesos en memoria por el momento ---\n");
		printf("-----------------------------------------------------------------\n");
		printf("\n\n");
	
	}
	
	
}

/*Función encargada de mostrar el proceso que está pidiendo memoria en el momento*/
void verProcesoPideMemoria(){

	int idProcesoPideMemoria;
	idProcesoPideMemoria = leerIdMemoriaCompartida("procesoPideMemoria.txt");
	procesoPideMemoria = shmat(idProcesoPideMemoria, NULL, 0);


	printf("--- Viendo el proceso que está pidiendo memoria en el momento ---\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	
	if(procesoPideMemoria[0].idProceso != -1){
		
		printf("Proceso: %i\n", procesoPideMemoria[0].idProceso);
		printf("Mecanismo: %s\n", procesoPideMemoria[0].tipoMecanismo);
	
	}

	else{
		printf("--- No hay ningún proceso pidiendo memoria por el momento ---\n");
		printf("-----------------------------------------------------------------\n");
		printf("\n\n");
	}

	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	
	

}

/*Función encargada de mostrar los procesos bloqueados en el momento*/
void verProcesosBloqueados(){

	int idProcesosBloqueados;
	int contador;
	idProcesosBloqueados = leerIdMemoriaCompartida("procesosBloqueados.txt");
	procesosBloqueados = shmat(idProcesosBloqueados, NULL, 0);
	

	contador = 0;

	printf("--- Viendo los procesos bloqueados en el momento ---\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	
	/*Se recorren los procesos bloqueados*/
	for(int i = 0; i<tamanioEspiaProcesosBloqueados; i++){

		if(procesosBloqueados[i].idProceso != -1){
			
			printf("Proceso: %i\n", procesosBloqueados[i].idProceso);
			printf("Tipo de mecanismo: %s\n", procesosBloqueados[i].tipoMecanismo);
			contador++;


		}

	}

	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	

	if(contador == 0){

		printf("--- No hay ningún proceso bloqueado por el momento ---\n");
		printf("-----------------------------------------------------------------\n");
		printf("\n\n");
	
	}
}

/*Función encargada de mostrar los procesos muertos en el momento*/
void verProcesosMuertos(){

	int contador;
	int idProcesosMuertos;
	idProcesosMuertos = leerIdMemoriaCompartida("procesosMuertos.txt");
	procesosMuertos = shmat(idProcesosMuertos, NULL, 0);
	contador = 0;

	printf("--- Viendo los procesos muertos en el momento ---\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");

	/*Se recorren los procesos muertos*/
	for(int i = 0; i<tamanioEspiaProcesosMuertos; i++){

		if(procesosMuertos[i].idProceso != -1){
			
			printf("Proceso: %i\n", procesosMuertos[i].idProceso);
			printf("Tipo de mecanismo: %s\n", procesosMuertos[i].tipoMecanismo);
			contador++;


		}

	}

	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	

	if(contador == 0){

		printf("--- No hay ningún proceso muerto por el momento ---\n");
		printf("-----------------------------------------------------------------\n");
		printf("\n\n");
	
	}
	

}

/*Función encargada de mostrar los procesos terminados en el momento*/
void verProcesosTerminados(){

	int contador;
	int idProcesosTerminados;
	idProcesosTerminados = leerIdMemoriaCompartida("procesosTerminados.txt");
	procesosTerminados = shmat(idProcesosTerminados, NULL, 0);
	contador = 0;

	printf("--- Viendo los procesos terminados en el momento ---\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	printf("-----------------------------------------------------------------\n");
	printf("\n\n");


	/*Se recorren los procesos terminados*/
	for(int i = 0; i<tamanioEspiaProcesosTerminados; i++){

		if(procesosTerminados[i].idProceso != -1){
			
			printf("Proceso: %i\n", procesosTerminados[i].idProceso);
			printf("Tipo de mecanismo: %s\n", procesosTerminados[i].tipoMecanismo);
			contador++;


		}

	}

	printf("-----------------------------------------------------------------\n");
	printf("\n\n");
	

	if(contador == 0){
		printf("--- No hay procesos terminados por el momento ---\n");
		printf("-----------------------------------------------------------------\n");
		printf("\n\n");
	
	}

}

void verTodo(){

	verEstadoMemoria();
	verProcesosEnMemoria();
	verProcesoPideMemoria();
	verProcesosBloqueados();
	verProcesosMuertos();
	verProcesosTerminados();

}

int main(){

	char *opcion; /*Aquí se almacenará el valor de entrada de la consola*/
	
	//return 0;

	int opcionN;  /*Aquí se almacenará el valor entero de la entrada a la consola, para el switch*/

	opcion = (char*)malloc(sizeof(char*));
	//contenidoBitacora =  (char*)malloc(sizeof(char*));
	//contenidoEspia =  (char*)malloc(sizeof(char*));


	printf("Bienvenido(a), este es el programa que se encargará de brindar información acerca de los procesos:\n");
	printf("****************************\n");
	printf("***    1-Estado de la memoria                         ***\n");
	printf("***    2-Procesos que estén en memoria                ***\n");
	printf("***    3-Proceso que se encuentre pidiendo memoria    ***\n");
	printf("***    4-Procesos bloqueados                          ***\n");
	printf("***    5-Procesos que han muerto                      ***\n");
	printf("***    6-Procesos que ya terminaron su ejecución      ***\n");
	printf("***    7-Ver todo                                     ***\n");
	printf("****************************\n");
	scanf("%s", opcion);

	
	opcionN = atoi(opcion); /*Conversión a tipo entero de la entrada en consola*/

	/*Control principal de las opciones, por este medio se llamará al modelo de paginación o segmentación*/
	switch(opcionN){

		case 1:
			printf("** Ha seleccionado ver el estado de la memoria **\n");
			verEstadoMemoria();
			break;

		case 2:
			printf("** Ha seleccionado ver los procesos que están en memoria **\n");
			verProcesosEnMemoria();
			break;

		case 3:
			printf("** Ha seleccionado ver el proceso que se encuentre pidiendo memoria **\n");
			verProcesoPideMemoria();
			break;

		case 4:
			printf("** Ha seleccionado ver los  procesos que se encuentran bloqueados **\n");
			verProcesosBloqueados();
			break;

		case 5:
			printf("** Ha seleccionado ver los procesos que han muerto **\n");
			verProcesosMuertos();
			break;

		case 6:
			printf("** Ha seleccionado ver los procesos que ya terminaron su ejecución **\n");
			verProcesosTerminados();
			break;

		case 7:
			printf("** Ha seleccionado ver toda la información acerca de los procesos **\n");
			verTodo();
			break;

		default:
			printf("No ha digitado una opción válida\n");	


	}


	return 1;
}