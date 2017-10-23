#include "Segmento.h"

typedef struct ProcesoSegmentacion{

	int idProceso; 
	unsigned long idThread;
	int tiempo;
	int numeroSegmentos;
	Segmento segmentos[5];  /*De 1 a 5 segmentos*/

	

} ProcesoSegmentacion;