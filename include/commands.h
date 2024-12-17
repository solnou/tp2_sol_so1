#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 *   @brief ejecuta un comando externo
 *   si el comando no se encuentra en la lista de comandos internos
 *   se interpreta que se esta invocando un programa. Este se va
 *   a ejecutar en un proceso hijo
 *   @param programa a ejecutar
 */
void external_command(char* command);

/*
 *  @brief Implementacion del comando cd
 *  Cambia el directorio de trabajo actual
 *  @param directorio nuevo
 *         NULL = se cambia al directorio HOME
 *        "-" = se cambia al directorio anterior
 *        path = se cambia al directorio "path"
 */
void command_cd(char* arg);

/*
 *  @brief Implementacion del comando echo
 *  Imprime un mensaje en la consola
 *  @param mensaje a imprimir
 */
void command_echo(char* arg);

/*
 *  @brief Implementacion del comando clr
 *  Limpia la consola
 */
void command_clear();

/*
 *  @brief Implementacion del comando quit
 *  Sale del programa
 */
void command_quit();

/**
 *  @brief Estructura para mapear comandos con sus implementaciones
 */
typedef struct
{
    char* name;              /**< Nombre del comando */
    void (*func)(char* arg); /**< Puntero a la función que implementa el comando */
} Command;

/**
 * @brief Lista de comandos internos
 */
extern Command internals_commands[];

#endif // COMMANDS_H
