#ifndef COMMANDS_H
#define COMMANDS_H

#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 *   @brief ejecuta un comando externo
 *   si el comando no se encuentra en la lista de comandos internos
 *   se interpreta que se esta invocando un programa. Este se va
 *   a ejecutar en un proceso hijo
 *   @param programa a ejecutar
 */
void external_command(char *command);

/**
 *  @brief Implementacion del comando cd
 *  Cambia el directorio de trabajo actual
 *  @param arg directorio al que se desea cambiar
 */
void command_cd(char *arg);

/**
 *  @brief Implementacion del comando echo
 *  Imprime un mensaje en la consola
 *  @param arg mensaje a imprimir
 */
void command_echo(char *arg);

/**
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
typedef struct {
  char *name; /**< Nombre del comando */
  void (*func)(
      char *arg); /**< Puntero a la función que implementa el comando */
} Command;

/**
 * @brief Lista de comandos internos
 */
extern Command internals_commands[];

/**
 * @brief Implementacion del comando ls
 *  Lista los archivos de un directorio
 *  @param arg directorio a listar
 */
void command_ls(char *arg);

/**
 * @brief Verifica si un archivo tiene una extensión de configuración
 * @param filename nombre del archivo
 */
bool es_archivo_configuracion(const char *filename);

/**
 * @brief Lista los archivos de un directorio
 * @param dir_name directorio a listar
 */
void listar_directorio(const char *dir_name);

/**
 * @brief Lee el contenido de un archivo de configuración y muestra por pantalla
 * @param filepath ruta del archivo
 */
void leer_archivo_configuracion(const char *filepath);

#endif // COMMANDS_H
