#ifndef EXECUTIONS_H
#define EXECUTIONS_H

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "commands.h"

/*
 *  @brief Funcion que ejecuta un comando en primer plano
 *  Separa los argumentos y define si el comando es interno o externo
 */
void execute_command(char* command);
/*
 *   @brief Funcion que ejecuta un comando en segundo plano
 *   Al detectar in & el comando se ejecuta en segundo plano  en
 *   un proceso hijo. Su salida se redirige a un archivo temporal,
 *   y el padre imprime el ID del trabajo y el ID del proceso mientras
 *   esta en ejecucion. UNa vez finalizada la ejecucion se imprime el file
 */
void execute_command_secondplane(char* command);

/*
 *  @brief Funcion que ejecuta comandos encadenados
 *  Separa los comandos en cada | y crea un pipe para cada par de comandos
 *  Redirige la salida del primer comando al pipe y la entrada del segundo comando al pipe
 */
void execute_piped_commands(char* command);

/*
 *  @brief Funcion que ejecuta un comando con redireccion de IO
 *  Redirige stdin de un archivo a un comando y desde stdout a un archivo
 *  FUnciona para comandos internos y externos
 */
void execute_command_redirection(char* command);

/*
 *  @brief Funcion que restaura la entrada y salida estandar
 *  Restaura la entrada y salida estandar despues de redirigirlas
 *  @param original_stdin, original_stdout, original_stderr son los archivos originales
 *         input_file, output_file son a los que fueron redirigidos para esa ejecucion
 */

void restore_io(int original_stdin, int original_stdout, int original_stderr);

/*
 *  @brief Funcion que maneja las señales
 *  Si se recibe una señal de interrupción, se envía la señal al proceso en primer plano
 * @param signo El número de la señal que se está manejando.
 */
void signal_handler(int signo);

/*
 *  @brief Funcion que asigna el handler a cada señal
 */
void setup_signals(void);

/*
 *  @brief Funcion que asigna el pid del proceso en primer plano
 *  @param pid El pid del proceso en primer plano
 */
void set_foreground_pid(pid_t pid);

#endif // EXECUTIONS_H
