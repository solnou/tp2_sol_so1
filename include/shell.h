#ifndef SHELL_H
#define SHELL_H

#include "../include/commands.h"
#include "../include/executions.h"
#include "../include/monitor.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/**
 * @brief Inicializa el shell y maneja las entradas.
 *
 * @param argc Número de argumentos.
 * @param argv Array de argumentos.
 * @return int Código de salida del programa.
 */
int init_shell(int argc, char* argv[]);

/**
 * @brief Imprime el prompt para cada nueva línea de comando.
 */
void prompt(void);

/**
 * @brief Elige el tipo de ejecución según la sintaxis del comando.
 *
 * @param command El comando a ejecutar.
 */
void choose_execution(char* command);

#endif // SHELL_H
