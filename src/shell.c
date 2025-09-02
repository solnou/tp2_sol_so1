#include "../include/shell.h"

void prompt(void);
void choose_execution(char *command);

/*
** Inicializacion del programa y manejo de entradas
*/
int init_shell(int argc, char *argv[]) {

  printf("╔════════════════════════════════════════════════╗\n");
  printf("║            Bienvenid@ a mi shell!              ║\n");
  printf("╚════════════════════════════════════════════════╝\n");

  setup_signals();

  if (argc > 1) { // se envio un bathc file
    // Leer comandos desde el archivo
    FILE *file =
        fopen(argv[1], "r"); // r indica que se va a leer (Read) el archivo
    if (file == NULL) {
      perror("Error al abrir el archivo");
      exit(EXIT_FAILURE);
    }

    char command[256];
    // el while ejecuta todos los comandos del archivo, fgets se encarga de
    // saltar de linea en cada bucle
    while (fgets(command, sizeof(command), file) != NULL) {
      // seleccionamos que tipo de ejecucion
      choose_execution(command);
    }
    fclose(file);
  } else {
    // Si se ejecuta sin argumentos
    char command[256];
    while (1) {
      prompt();
      if (fgets(command, sizeof(command), stdin) != NULL) {
        choose_execution(command);
      }
    }
  }
  return 0;
}
/*
 * Funcion que imprime el prompt
 * para cada nueva linea de comando
 */
void prompt() {
  char *user = getenv("USER");
  if (user == NULL) {
    user = "unknown";
  }

  char hostname[1024];
  if (gethostname(hostname, sizeof(hostname)) != 0) {
    perror("gethostname");
    exit(EXIT_FAILURE);
  }

  char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("getcwd");
    exit(EXIT_FAILURE);
  }

  printf("%s@%s:%s$ ", user, hostname, cwd);
}

/*
 * Funcion que elige el tipo de ejecucion
 * segun la sintaxis del comando
 */
void choose_execution(char *command) {

  command[strcspn(command, "\n")] = 0; // Eliminar el salto de línea

  if (command[strlen(command) - 1] ==
      '&') { // verifica si el ultimo caracter es un &

    execute_command_secondplane(command);
  } else if (strchr(command, '|') !=
             NULL) // verifica si hay alguna | en el codigo
  {

    execute_piped_commands(command);
  } else if (strchr(command, '<') != NULL ||
             strchr(command, '>') !=
                 NULL) // verifica si hay algun <> en el codigo
  {

    execute_command_redirection(command);
  } else { // si no hay ningun simbolo especial, es un comando normal

    execute_command(command);
  }
}
