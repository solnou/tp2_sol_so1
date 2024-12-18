#include "../include/commands.h"
#include "../include/monitor.h"

// Definición del arreglo de comandos internos
Command internals_commands[] = {
    {"cd", command_cd},
    {"echo", command_echo},
    {"clr", (void (*)(char*))command_clear},
    {"quit", (void (*)(char*))command_quit},
    {"start_monitor", start_monitor},
    {"stop_monitor", stop_monitor},
    {"status_monitor", status_monitor},
    {"ls", command_ls}
};

/*En el caso de que no se llame a ninguno de los comandos listados en el arreglo de internals_commands
, se ejecutará la función external_command
*/
void external_command(char* command)
{

    // Crear un proceso hijo
    pid_t pid = fork();

    switch (pid)
    {
    // Hubo un error al crear el proceso hijo
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        break;
    // Se creo correctamente el proceso hijo
    case 0: {
        // Separar el comando y sus argumentos
        char* args[256];
        int i = 0;
        args[i] = strtok(command, " ");
        while (args[i] != NULL)
        {
            i++;
            args[i] = strtok(NULL, " ");
        }

        // Ejecutar el programa
        if (execvp(args[0], args) == -1)
        {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        break;
    }

    // Proceso padre
    default:
        // Esperar a que el proceso hijo termine
        waitpid(pid, NULL, 0);
        break;
    }
}

// COMANDOS INTERNOS
void command_cd(char* arg)
{
    char* directory;
    char cwd[1024];
    char* oldpwd = getenv("PWD");

    if (arg == NULL || strcmp(arg, "") == 0)
    {
        directory = getenv("HOME");
        if (directory == NULL)
        {
            fprintf(stderr, "cd: HOME no está definido\n");
            return;
        }
    }
    else if (strcmp(arg, "-") == 0)
    {
        directory = getenv("OLDPWD");
        if (directory == NULL)
        {
            fprintf(stderr, "cd: OLDPWD no está definido\n");
            return;
        }
        printf("%s\n", directory);
    }
    else
    {
        directory = arg;
    }

    if (chdir(directory) != 0)
    {
        perror("cd");
        return;
    }

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        setenv("OLDPWD", oldpwd, 1);
        setenv("PWD", cwd, 1);
    }
    else
    {
        perror("getcwd");
    }
}

void command_echo(char* arg)
{
    if (arg == NULL)
    {
        printf("\n");
        return;
    }

    if (arg[0] == '$')
    {
        char* env_var = getenv(arg + 1);
        if (env_var != NULL)
        {
            printf("%s\n", env_var);
        }
        else
        {
            printf("Variable de entorno no encontrada: %s\n", arg + 1);
        }
    }
    else
    {
        printf("%s\n", arg);
    }
}

void command_clear()
{
    printf("\033[H\033[J");
}

void command_quit()
{
    exit(0);
}

void command_ls(char* arg)
{
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char *dir_name = arg > 1 ? arg[1] : ".";  // Directorio a listar


    dir = opendir(dir_name);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Lee cada archivo en el directorio
    while ((entry = readdir(dir)) != NULL) {
        // Verifica si el archivo tiene una extensión de configuración
        if (es_archivo_configuracion(entry->d_name)) {
            // Obtiene el estado del archivo
            if (stat(entry->d_name, &file_stat) == -1) {
                perror("stat");
                continue;
            }

            // Muestra el nombre del archivo de configuración
            printf("%s\n", entry->d_name);
        }
    }

    // Cierra el directorio
    closedir(dir);
    return 0;

}

bool es_archivo_configuracion(const char *filename) {
    const char *config_extensions[] = {".conf", ".cfg", ".ini"};
    size_t num_extensions = sizeof(config_extensions) / sizeof(config_extensions[0]);

    for (size_t i = 0; i < num_extensions; i++) {
        size_t len_ext = strlen(config_extensions[i]);
        size_t len_filename = strlen(filename);

        if (len_filename >= len_ext && strcmp(filename + len_filename - len_ext, config_extensions[i]) == 0) {
            return true;
        }
    }

    return false;
}
