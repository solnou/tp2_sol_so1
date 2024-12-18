#include "../include/executions.h"

// Variable global para indicar el id del trabajo
static int job_id = 1;
static int size_internal_commands = 8;
pid_t foreground_pid = 0;

// Declaración de la funciones
void execute_command(char* command);
void execute_command_secondplane(char* command);
void execute_piped_commands(char* command);

// EJECCUCION DE COMANDOS EN PRIMER PLANO
// --------------------------------------------------------------------------------
void execute_command(char* command)
{

    // Separamos el comando y sus argumentos
    char* command_name = strtok(command, " ");
    char* arg = strtok(NULL, " ");

    // Buscamos el comando en la lista de comandos internos
    for (int i = 0; i < size_internal_commands; i++)
    {
        if (strcmp(command_name, internals_commands[i].name) == 0)
        {
            internals_commands[i].func(arg);
            return;
        }
    }
    // En el caso de que no se llame a ninguno de los comandos listados en el arreglo de internals_commands
    external_command(command);

    // Si no se encuentra el comando, imprimir un mensaje de error
    printf("Comando no encontrado: %s\n", command_name);

    return;
}

// EJECCUCION DE COMANDOS EN SEGUNDO PLANO
// ------------------------------------------------------------------------------
void execute_command_secondplane(char* command)
{

    // Eliminar el '&'
    command[strlen(command) - 1] = '\0'; // \0 simbolo de fin de cadena

    pid_t pid = fork();

    if (pid == 0)
    { // Proceso hijo

        // Guardar los descriptores de archivo originales
        int original_stdin = dup(STDIN_FILENO);
        int original_stdout = dup(STDOUT_FILENO);
        int original_stderr = dup(STDERR_FILENO);

        // Mnadamos la salida a un archivo temporal
        if (freopen("/tmp/command_output.txt", "w", stdout) == NULL)
        {
            perror("freopen failed");
        }
        if (freopen("/tmp/command_output.txt", "w", stderr) == NULL)
        {
            perror("freopen failed");
        }

        // Ejecutar el comando
        execute_command(command);

        // Restaurar la entrada y salida originales
        restore_io(original_stdin, original_stdout, original_stderr);

        // Cerrar los descriptores duplicados
        close(original_stdin);
        close(original_stdout);
        close(original_stderr);

        exit(0); // Asegurarse de que el proceso hijo termine
    }
    else if (pid > 0)
    { // Proceso padre
        set_foreground_pid(pid);
        // Imprimir el ID del trabajo y el ID del proceso
        printf("[%d] %d\n", job_id++, pid);

        // Esperar a que el proceso hijo termine

        int status;
        waitpid(pid, &status, 0);

        // Leer y mostrar la salida del archivo temporal
        FILE* file = fopen("/tmp/command_output.txt", "r");
        if (file != NULL)
        {
            char line[256];
            while (fgets(line, sizeof(line), file) != NULL)
            {
                printf("%s", line);
            }
            fclose(file);
        }
    }
    else
    { // Si el pid es -1
        // Error al crear el proceso hijo

        perror("fork");
    }
    set_foreground_pid(0);
}

// EJECUCION DE COMANDOS ENCADENADOS CON
// PIPE-----------------------------------------------------------------------------------
void execute_piped_commands(char* command)
{
    // 1. Dividir los comandos en cada |
    char* commands[20]; // Arreglo para todos los comandos resultantes
    int num_commands = 0;
    char* command_n = strtok(command, "|");
    while (command_n != NULL)
    { // mientras haya | para separar, va dividiendo
        commands[num_commands++] = command_n;
        command_n = strtok(NULL, "|");
    }

    // 2. Crear el pipe para cada par de comandos
    int* filedes = malloc(2 * (num_commands - 1) * sizeof(int)); // Usar memoria dinámica
                                                                 // lea y ottro que escriba hacemos *2
    for (int j = 0; j < num_commands - 1; j++)
    {
        if (pipe(filedes + j * 2) < 0)
        { // Crea los pipes y veriifica que no fallen
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // 3. Redirigir la salida del primer comando al pipe
    int j = 0;
    for (int i = 0; i < num_commands; i++)
    {
        // Crear un proceso hijo para cada comando
        pid_t pid = fork();
        if (pid == 0)
        {
            if (i < num_commands - 1)
            {
                // Redirigimos la salida estandar al pipe file descriptor de escritura
                if (dup2(filedes[j + 1], STDOUT_FILENO) < 0)
                {
                    // Si falla la redirección, imprimir un mensaje de error
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            // 4. Redirigir la entrada del segundo comando al pipe

            if (j != 0)
            {
                // Saltenado el primer comando, redirigimos la entrada estandar al pipe file descriptor de lectura
                if (dup2(filedes[j - 2], STDIN_FILENO) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }
            for (int k = 0; k < 2 * (num_commands - 1); k++)
            {
                // luego de redirigir los file descriptors, cerramos los pipes
                close(filedes[k]);
            }
            // 5. EJecutamos los comandos
            // Separamos los argumentos del comando
            char* arguments[10];
            char* cmd_token = strtok(commands[i], " ");
            int arg_count = 0;
            while (cmd_token != NULL)
            {
                arguments[arg_count++] = cmd_token;
                cmd_token = strtok(NULL, " ");
            }
            arguments[arg_count] = NULL;
            // ejecutamos
            execvp(arguments[0], arguments);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        { // problemas al crear el proceso hijo
            perror("fork");
            exit(EXIT_FAILURE);
        }
        j += 2;
    }
    for (int i = 0; i < 2 * (num_commands - 1); i++)
    {
        close(filedes[i]);
    }
    for (int i = 0; i < num_commands; i++)
    {
        wait(NULL);
    }
}

// EJECUCION CON REDIREECCION DE SALIDA -----------------------------------------------------------------------
void execute_command_redirection(char* command)
{

    // Separamos las partes del comando
    char* input_file = NULL;
    char* output_file = NULL;
    char* program = NULL;
    char* args[100];
    int arg_count = 0;

    // Asignamos cada parte del comando a su variable
    char* token = strtok(command, " ");
    while (token != NULL)
    {
        if (strcmp(token, "<") == 0)
        { // si encontramos < el siguiente token es el archivo de entrada
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                input_file = token;
            }
        }
        else if (strcmp(token, ">") == 0)
        { // si encontramos > el siguiente token es el archivo de salida
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                output_file = token;
            }
        }
        else
        {
            if (program == NULL)
            { // el primer token, sin haber encontrado ningun <>, es el programa
                program = token;
            }
            args[arg_count++] = token; // los demas tokens son argumentos
        }
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL; // finalizamos la lista con un NULL

    // Guardar los descriptores de archivo originales
    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);
    int original_stderr = dup(STDERR_FILENO);

    // Redirigir la entrada si se especifica el archivo luego de <
    if (input_file != NULL)
    {
        int input_fd = open(input_file, O_RDONLY);
        if (input_fd < 0)
        {
            perror("open input file");
            exit(EXIT_FAILURE);
        }
        dup2(input_fd, STDIN_FILENO);
        close(input_fd);
    }

    // Redirigir la salida si se especifica el archivo luego de >
    if (output_file != NULL)
    {
        int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd < 0)
        {
            perror("open output file");
            exit(EXIT_FAILURE);
        }
        dup2(output_fd, STDOUT_FILENO);
        dup2(output_fd, STDERR_FILENO);
        close(output_fd);
    }

    // Ejecutar el comando
    execute_command(program);

    // Restaurar la entrada y salida originales
    restore_io(original_stdin, original_stdout, original_stderr);
    close(original_stdin);
    close(original_stdout);
    close(original_stderr);
}

// RESTAURAR LA DIRECCION DE ENTRADA Y SALIDA -----------------------------------------------------------------------
void restore_io(int original_stdin, int original_stdout, int original_stderr)
{
    dup2(original_stdin, STDIN_FILENO);
    dup2(original_stdout, STDOUT_FILENO);
    dup2(original_stderr, STDERR_FILENO);
}

void signal_handler(int signo)
{
    if (foreground_pid > 0)
    {
        kill(foreground_pid, signo);
    }
}

void setup_signals()
{
    signal(SIGINT, signal_handler);  // Signal for interrupt (CTRL+C)
    signal(SIGTSTP, signal_handler); // Signal for stop (CTRL+Z)
    signal(SIGQUIT, signal_handler); // Signal for quit (CTRL+\)
}

void set_foreground_pid(pid_t pid)
{
    foreground_pid = pid;
}
