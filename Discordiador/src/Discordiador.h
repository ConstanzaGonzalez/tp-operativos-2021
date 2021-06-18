#ifndef DISCORDIADOR_H_
#define DISCORDIADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/log.h"
#include "commons/string.h"
#include "commons/config.h"
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>

#include "utils/sockets.h"
#include "utils/loader.h"
#include "utilsDiscordiador.h"
#include "planificador.h"
#include "utils/estructuras.h"

#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Discordiador/discordiador.config"

char** parser_consola;
char** parser_posiciones;
uint32_t operacion;

// Conexiones
char* IP_MI_RAM;
char* PUERTO_MI_RAM;
char* IP_MONGO_STORE;
char* PUERTO_MONGO_STORE;
int32_t conexion_mongo_store;
int32_t conexion_mi_ram;

// Data del Config
//int GRADO_MULTITAREA;
//char* ALGORITMO;
//int QUANTUM;
int DURACION_SABOTAJE;
int RETARDO_CICLO_CPU;

//int32_t conexion_socket;
int32_t conexion_sabotaje;

// Semaforos
sem_t* comando_para_ejecutar;
sem_t* sabotaje;
sem_t* termino_operacion;

// Hilos
pthread_t hilo_consola;
pthread_t hilo_sabotaje;
pthread_t hilo_tripulante;
pthread_t hilo_new_ready;
pthread_t hilo_ready_running;
pthread_t hilo_solucion;   //preguntar

pthread_t hilo_sabotaje;



t_log* logger;
t_config* config;

//void iniciar_escucha_sabotaje(void);
void iniciar_escucha_por_consola();
void obtener_datos_de_config(t_config* config);
void obtener_orden_input();						// Leo lo que escriba por consola y ejecuta la operacion a realizar
void crear_hilos();
void iniciar_escucha_sabotaje();

void esperadorDeUno(sem_t* semaforo);
void solucionadorProblemas(sem_t*);

void poner_en_cero_semaforos(sem_t* semaforo);
void poner_en_uno_semaforos(sem_t* semaforo);

bool menorId(tripulante_plani* tripulante1,tripulante_plani* tripulante2);
// POR AHORA SON UNA IDEA

void arreglar_sabotaje(void);

t_pcb* crear_pcb(void);

// Colas de planificacion
//t_queue* colaNew;

#endif /* DISCORDIADOR_H_ */
