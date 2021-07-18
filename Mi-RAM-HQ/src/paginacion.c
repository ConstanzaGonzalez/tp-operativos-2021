#include "paginacion.h"
#include <math.h>

int32_t cantidad_paginas_usadas(int32_t tamanio) {

    int32_t cantidad_paginas = 0;

    while((cantidad_paginas * TAMANIO_PAGINA) < tamanio){
    	cantidad_paginas++;
    }

    return cantidad_paginas;
}


void inicializar_frames(void) {
	cantidad_frames = TAMANIO_MEMORIA / TAMANIO_PAGINA;
	log_info(logger, "Se inician %u frames en Memoria Principal.", cantidad_frames);
	frames = malloc(sizeof(frame) * cantidad_frames);
	for(int i=0; i<cantidad_frames; i++) {
		frames[i] = malloc(sizeof(frame));
		frames[i]->estado = LIBRE;
		frames[i]->espacio_libre = TAMANIO_PAGINA;
		frames[i]->pagina = -1;
		frames[i]->proceso = -1;
	}
}


int32_t obtener_siguiente_frame(t_list* paginas, int32_t contador) {
	t_pagina* pagina_buscada = list_get(paginas, contador);
	return pagina_buscada->numero_de_frame;
}


int32_t obtener_frame_disponible(void) {
	int32_t num_frame;
	if(hay_frame_libre() == 1) {
		num_frame = obtener_frame_libre();
		return num_frame;
	}
	else {
		//APLICAR ALGORITMOS DE REEMPLAZO (LRU o CLOCK)
		//PARA OBTENER UN FRAME PARA GUARDAR (LO DEMAS SE GUARDA EN SWAP)
		return -1;
	}
}

int hay_frame_libre(void) {
	for(int i=0; i<cantidad_frames; i++) {
		if(frames[i] == LIBRE) {
			return 1;
		}
	}
	return 0;
}

int32_t obtener_frame_libre(void) {
	for(int i=0; i<cantidad_frames; i++) {
		if(frames[i] == LIBRE) {
			return i;
		}
	}
	return -1;
}


t_tabla_paginas_patota* crear_tabla_paginas(void) {

	t_tabla_paginas_patota* tabla = malloc(sizeof(t_tabla_paginas_patota));
	tabla->paginas = list_create();
	tabla->direccion_tripulantes = list_create();
	return tabla;
}




void iniciar_tabla_patota(t_tabla_paginas_patota* tabla_patota, int32_t tamanio_total, tareas_patota* tareas_de_la_patota, int32_t cantidad_tripulantes) {

	int32_t cantidad_paginas = cantidad_paginas_usadas(tamanio_total);

	for(int i=0; i<cantidad_paginas; i++) {
		t_pagina* pagina = malloc(sizeof(t_pagina));
		pagina->numero_de_pagina = contador_pagina;
		pagina->P = 0;
		pagina->numero_de_frame = -1;
		pagina->U = 0;

		list_add_in_index(tabla_patota->paginas, i, pagina);

		contador_pagina++;
	}

	tabla_patota->direccion_patota = puntero_inicio;
	puntero_inicio += tamanio_patota;

	//tabla_patota->direccion_tareas = puntero_inicio;
	puntero_inicio += tareas_de_la_patota->tamanio_tareas;


	int32_t tamanio_tripulantes = 0;

	for(int c=0; c<cantidad_tripulantes; c++) {

		uint32_t direccion_tripulante = puntero_inicio;
		puntero_inicio += tamanio_tripulante;

		tamanio_tripulantes += tamanio_tripulante;
		list_add_in_index(tabla_patota->direccion_tripulantes, c, direccion_tripulante);
	}

	if(tamanio_total == (tamanio_patota + tareas_de_la_patota->tamanio_tareas + tamanio_tripulantes)) {
		printf("La patota entera pesa %u bytes.\n", tamanio_total);
	}
}


void guardar_patota_en_memoria(void* estructura, tipo_estructura tipo, t_tabla_paginas_patota* tabla_patota, int32_t tamanio_total) {


	int32_t pid;
	int32_t desplazamiento = 0;
	int32_t paginas_necesarias = list_size(tabla_patota->paginas);

	void* buffer = malloc(tamanio_total);

	switch(tipo) {

		case PATOTA:
			break;
		case TAREAS:
			break;
		case TRIPULANTE:
			break;
	}


	int offset = 0;

	int32_t sobrante = tamanio_total - (paginas_necesarias-1) * TAMANIO_PAGINA;

	for(int c=0; c<paginas_necesarias; c++) {

		//int32_t num_frame = obtener_siguiente_frame(tabla_patota->paginas, contador);
		t_pagina* pagina_buscada = list_get(tabla_patota->paginas, c);
		asignar_frame_disponible(pagina_buscada, pid);

		int32_t num_frame = pagina_buscada->numero_de_frame;

		int32_t inicio = num_frame * TAMANIO_PAGINA;

		if(paginas_necesarias == 1) {
			memcpy(memoria_principal + inicio, buffer + offset, sobrante);
			offset += sobrante;
		}
		else {
			memcpy(memoria_principal + inicio, buffer + offset, TAMANIO_PAGINA);
			offset += TAMANIO_PAGINA;
		}
	}

}


void asignar_frame_disponible(t_pagina* pagina, uint32_t pid) {

	pagina->numero_de_frame = obtener_frame_disponible();
	pagina->tiempo_referencia = temporal_get_string_time("%H:%M:%S:%MS");
	frame* frame_ocupado = frames[pagina->numero_de_frame];
	frame_ocupado->pagina = pagina->numero_de_pagina;
	frame_ocupado->proceso = pid;
}

t_tabla_paginas_patota* buscar_tabla_patota(uint32_t id_patota) {

	bool se_encuentra_patota(void* tabla){
		return ((t_tabla_paginas_patota*)tabla)->patota->pid == id_patota;
	}

	t_tabla_paginas_patota* tabla_buscada = list_find(tablas_paginas, se_encuentra_patota);

	return tabla_buscada;
}

int32_t buscar_pagina_por_id(t_tabla_paginas_patota* tabla_patota_buscada, uint32_t id_tripulante_a_buscar) {


	//buscar direccion logica con el id del tripulante
	//a partir de la direc logica sacar pagina y luego offset

	t_list* direccion_tripulantes = tabla_patota_buscada->direccion_tripulantes;

	bool mismo_id_tripulante(void* dl_tripulante) {
		return (((t_dl_tripulante*)dl_tripulante)->id_tripulante == id_tripulante_a_buscar);
	}

	t_dl_tripulante* direc_logica_tripulante = list_find(direccion_tripulantes, mismo_id_tripulante);

	return direc_logica_tripulante->direccion_logica;
}

int32_t obtener_direc_fisica_con_direccion_logica(int32_t direccion_logica, t_tabla_paginas_patota* tabla_patota_buscada) {
	int32_t numero = direccion_logica / TAMANIO_PAGINA;

	int32_t entera = (int32_t) numero;

	int32_t resto = direccion_logica % TAMANIO_PAGINA;

	int32_t numero_de_frame = buscar_frame(entera, tabla_patota_buscada->paginas);

	int32_t inicio = numero_de_frame * TAMANIO_PAGINA;

	return inicio + resto;

}

int32_t buscar_frame(int32_t nro_pagina, t_list* paginas){
	bool se_encuentra_pagina(void* pagina){
		return ((t_pagina*)pagina)->numero_de_pagina == nro_pagina;
	}

	t_pagina* pagina = list_find(paginas, se_encuentra_pagina);

	return pagina->numero_de_frame;
}

t_tcb* encontrar_tripulante_memoria(int32_t direccion_fisica) {

	t_tcb* tripulante = malloc(sizeof(t_tcb));

	void* inicio = (void*) direccion_fisica;
	uint32_t desplazamiento = 0;

	memcpy(&(tripulante->id_tripulante), inicio + desplazamiento, sizeof(tripulante->id_tripulante));
	desplazamiento += sizeof(tripulante->id_tripulante);

	memcpy(&(tripulante->estado_tripulante), inicio + desplazamiento, sizeof(tripulante->estado_tripulante));
	desplazamiento += sizeof(tripulante->estado_tripulante);

	memcpy(&(tripulante->posicion_x), inicio + desplazamiento, sizeof(tripulante->posicion_x));
	desplazamiento += sizeof(tripulante->posicion_x);

	memcpy(&(tripulante->posicion_y), inicio + desplazamiento, sizeof(tripulante->posicion_y));
	desplazamiento += sizeof(tripulante->posicion_y);

	memcpy(&(tripulante->id_tarea_a_realizar), inicio + desplazamiento, sizeof(tripulante->id_tarea_a_realizar));
	desplazamiento += sizeof(tripulante->id_tarea_a_realizar);

	memcpy(&(tripulante->puntero_PCB), inicio + desplazamiento, sizeof(tripulante->puntero_PCB));
	desplazamiento += sizeof(tripulante->puntero_PCB);

	return tripulante;
}

/*
void administrar_guardar_patota(t_tabla_paginas_patota* tabla_patota, int32_t tamanio_total, tareas_patota* tareas_de_la_patota, int32_t cantidad_tripulantes) {



	t_pagina* pagina_buscada = list_get(tabla_patota->paginas, 0);
	int32_t offset;



	offset = 0;
	tabla_patota->direccion_patota = pagina_buscada->numero_de_pagina * TAMANIO_PAGINA + offset;
	offset += tamanio_patota;

	tabla_patota->direccion_tareas = pagina_buscada->numero_de_pagina * TAMANIO_PAGINA + offset;
	offset += tareas_de_la_patota->tamanio_tareas;




	// DIRECCION LOGICA = NUM PAGINA * TAMANIO_PAGINA + OFFSET
	// DIRECCION FISICA = NUM FRAME * TAMANIO_FRAME + OFFSET

	pagina_buscada = list_get(tabla_patota->paginas, 0);
	offset = tabla_patota->direccion_patota - pagina_buscada->numero_de_pagina * TAMANIO_PAGINA;

	pagina_buscada->numero_de_frame = obtener_frame_disponible();

	int32_t direccion_fisica = pagina_buscada->numero_de_frame * TAMANIO_PAGINA + offset;
	int32_t inicio = direccion_fisica;

	// Creo PCB
	t_pcb* nueva_patota = crear_pcb();
	int32_t tamanio_guardado;

	void* buffer;

	memcpy(buffer + inicio, &(nueva_patota->pid), sizeof(nueva_patota->pid));
	inicio += sizeof(nueva_patota->pid);
	tamanio_guardado += sizeof(nueva_patota->pid);

	memcpy(memoria_principal + inicio, &(nueva_patota->tareas), sizeof(nueva_patota->tareas));
	inicio += sizeof(nueva_patota->tareas);
	tamanio_guardado += sizeof(nueva_patota->tareas);


	frame* frame_usado = frames[pagina_buscada->numero_de_frame];
	frame_usado->espacio_libre = TAMANIO_PAGINA - tamanio_guardado;

	int32_t tamanio_a_guardar;

	if(tamanio_a_guardar > frame_usado->espacio_libre) {

		int32_t restante = tamanio_a_guardar - frame_usado->espacio_libre;
		int32_t desplazamiento = 0;

		memcpy(memoria_principal + inicio, tareas_de_la_patota->tareas + desplazamiento, frame_usado->espacio_libre);
		desplazamiento += frame_usado->espacio_libre;

		inicio = obtener_frame_disponible() * TAMANIO_PAGINA;

		memcpy(memoria_principal + inicio, tareas_de_la_patota->tareas + desplazamiento, restante);

		frame* nuevo_frame = frames[inicio];
		nuevo_frame->espacio_libre = TAMANIO_PAGINA - restante;

	}

	memcpy(memoria_principal + inicio, buffer, sizeof(nueva_patota->pid));
	inicio += sizeof(nueva_patota->pid);





	*
	 * OBTENES DL
	 * OBTENES NUMERO PAGINA Y OFFSET
	 * BUSCAS PAGINAS EN LA PATOTA (TLB), UNA VEZ QUE ENCONTRAS LA PAGINA
	 *  OBTENES EL NUMERO DE FRAME
	 *
	 *  if(PAGINA TIENE P=1) {
	 *  	DIRECCION FISICA = NUMERO FRAME * TAMANIO_PAGINA + OFFSET
	 *  	Y AHI OBTENGO EL TRIPULANTE
	 *  }
	 *  else {
	 *  	TENGO QUE BUSCAR LA PAGINA EN DISCO Y MANDARLA A MP CON EL ALGORITMO
	 *  }



	 * Cuando se asigna un Frame => se guarda en memoria
	 *




	uint32_t desplazamiento = 0;
	int32_t pagina_actual = 0; 		// if(desplazamiento == TAMANIO_PAGINA) -> pagina_actual++;

	tabla_patota->direccion_patota = pagina_actual * TAMANIO_PAGINA + desplazamiento;

	memcpy(memoria_principal + desplazamiento, &(nueva_patota->pid), sizeof(nueva_patota->pid));
	desplazamiento += sizeof(nueva_patota->pid);

	memcpy(memoria_principal + desplazamiento, &(nueva_patota->tareas), sizeof(nueva_patota->tareas));
	desplazamiento += sizeof(nueva_patota->tareas);


	tabla_patota->direccion_tareas = pagina_actual * TAMANIO_PAGINA + desplazamiento;

	memcpy(memoria_principal + desplazamiento, tareas_de_la_patota->tareas, tareas_de_la_patota->tamanio_tareas);
	desplazamiento += tareas_de_la_patota->tamanio_tareas;


	void* buffer = malloc(tamanio_total);

	tabla_patota->direccion_patota = desplazamiento;

	memcpy(buffer + desplazamiento, &(nueva_patota->pid), sizeof(nueva_patota->pid));
	desplazamiento += sizeof(nueva_patota->pid);

	memcpy(buffer + desplazamiento, &(nueva_patota->tareas), sizeof(nueva_patota->tareas));
	desplazamiento += sizeof(nueva_patota->tareas);

	tabla_patota->direccion_tareas =

	memcpy(buffer + desplazamiento, tareas_de_la_patota->tareas, tareas_de_la_patota->tamanio_tareas);
	desplazamiento += tareas_de_la_patota->tamanio_tareas;


	desplazamiento = 0;
	int32_t paginas_necesarias = cantidad_paginas;


	int offset = 0;

	int32_t sobrante = tamanio_total - (paginas_necesarias-1) * TAMANIO_PAGINA;
	// CON 3 PAGINAS

	int contador = 0;
	while(paginas_necesarias > 0) {

		int32_t num_frame = obtener_siguiente_frame(tabla_patota->paginas, contador);

		int32_t inicio = num_frame * TAMANIO_PAGINA;

		if(paginas_necesarias == 1) {
			memcpy(memoria_principal + inicio, buffer + offset, sobrante);
			offset += sobrante;
		}
		else {
			memcpy(memoria_principal + inicio, buffer + offset, TAMANIO_PAGINA);
			offset += TAMANIO_PAGINA;
		}

		contador++;
		paginas_necesarias--;
	}
}*/


/*
 * 1) Hay alguna forma de validar si se puede guardar una patota con sus tripulantes?
 * 		- Se debe a que si no hay suficiente memoria, ¿podría guardarlo en la memoria SWAP?
 *
 * 2) Suponiendo que valida el tamaño:
 * 		- En un Frame guardaria un PCB y una parte de las tareas, y el sig Frame guardo el resto de las tareas
 *
 * 	Ejemplo: Tamaño Pagina: 32 bytes
 * 		Patota con 3 tripulantes y 2 tareas; cada Tarea = 20 bytes, cada Tripulante = 21 bytes y cada Pcb = 8 bytes
 * 		Tamaño Total de Patota = 111 bytes
 *
 * 		Utilizaria (aprox) 3,5 paginas
 *
 * 		Por lo tanto, utilizaria 3 frames completos y la mitad de otro para guardar una patota completa.
 *
 */






// FUNCIONES PARA ORDENAR O USAR EN LISTAS
bool menor_a_mayor_por_frame(void* pagina, void* pagina_siguiente) {
	return ((t_pagina*)pagina)->numero_de_frame < ((t_pagina*)pagina_siguiente)->numero_de_frame;
}
