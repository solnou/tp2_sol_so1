#include "commands.h"
#include "executions.h"
#include <cjson/cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Inicia el proceso de monitorización.
 *
 * Esta función crea un proceso en segundo plano para ejecutar el monitor de métricas.
 * Redirige stdout y stderr a `/dev/null` en el proceso hijo.
 */
void start_monitor();

/**
 * @brief Detiene el proceso de monitorización.
 *
 * Esta función envía una señal `SIGTERM` al proceso de monitorización en segundo plano,
 * detiene el proceso y restablece el estado de las variables `monitor_pid` y `monitoring`.
 */
void stop_monitor();

/**
 * @brief Muestra el estado de la monitorización de métricas.
 *
 * Esta función ofrece un menú interactivo para seleccionar métricas específicas o todas
 * las métricas disponibles. Lee datos desde un FIFO en formato JSON y presenta las métricas
 * en función de la opción seleccionada.
 */
void status_monitor();
