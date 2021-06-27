#ifndef MI_RAM_H_
#define MI_RAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include "utils/sockets.h"
#include "utils/loader.h"
#include "utils/estructuras.h"
#include "utils/tareas.h"
#include "memoria.h"

#define IP "127.0.0.1"
#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Mi-RAM-HQ/Mi-RAM.config"

// Config y Log
t_config* config;
t_log* logger;

// Datos del Config
char* PUERTO;
uint32_t TAMANIO_MEMORIA;
char* ESQUEMA_MEMORIA;
uint32_t TAMANIO_PAGINA;
uint32_t TAMANIO_SWAP;
char* PATH_SWAP;
char* ALGORITMO_REEMPLAZO;
char* CRITERIO_SELECCION;

void* memoria_principal;
void* area_swap;

char esquema_elegido;
criterio_seleccion criterio_elegido;
algoritmo_reemplazo algoritmo_elegido;

// Contadores
uint32_t contador_segmento;
uint32_t contador_id_tripu;
uint32_t contador_id_patota;

uint32_t base_segmento;
int32_t memoria_restante;
int32_t memoria_libre_por_segmento;
int32_t memoria_compactada;				// memoria_compactada = memoria_restante + memoria_libre_por_segmento;

uint32_t tamanio_tripulante;
uint32_t tamanio_tripulantes;
uint32_t tamanio_patota;
uint32_t tamanio_tareas;


sem_t* crear_segmento_sem;

sem_t* espera;

pthread_t hilo_recibir_mensajes;
pthread_mutex_t mutexTablasDeSegmentos;

t_list* ids;
char** parser_posiciones;

uint32_t cantidad_tareas(char** parser_tarea);

void iniciar_comunicacion(void);
void obtener_datos_de_config(t_config* config);
void procesar_mensajes(codigo_operacion operacion, int32_t conexion);

bool validar_espacio_por_patota_segmentacion(uint32_t tamanio);

t_pcb* crear_pcb(void);
t_tcb* crear_tcb(uint32_t dir_logica_pcb, uint32_t posicion_x, uint32_t posicion_y);

void mostrar_tripulante(t_tcb* tripulante);

#endif /* MI_RAM_H_ */
