#include "../include/monitor.h"

static pid_t monitor_pid = -1;
static int monitoring = 0;

void start_monitor() {
  if (monitor_pid == -1) // Verifica si el monitor no está en ejecución
  {
    monitor_pid = fork(); // Crea un nuevo proceso
    if (monitor_pid == 0) // Proceso hijo
    {
      // Proceso hijo: solo ejecuta el monitor
      int devnull =
          open("/dev/null", O_RDWR); // Abre /dev/null para redirigir la salida
      if (devnull != -1)             // Si se abrió correctamente
      {
        dup2(devnull, STDOUT_FILENO); // Redirige stdout a /dev/null
        dup2(devnull, STDERR_FILENO); // Redirige stderr a /dev/null
        close(devnull); // Cierra el descriptor de archivo de /dev/null
      }

      // Ejecuta el programa del monitor
      execlp("../lib/tp1_sol_so1/build/tp1_sol_so1", "tp1_sol_so1", NULL);
      perror("execlp failed"); // Si execlp falla, imprime un mensaje de error
      exit(EXIT_FAILURE);      // Termina el proceso hijo
    } else                     // Proceso padre
    {
      monitoring = 1;                // Marca que el monitor está en ejecución
      printf("Monitor iniciado.\n"); // Imprime un mensaje indicando que el
                                     // monitor ha iniciado
    }
  } else // Si el monitor ya está en uso
  {
    printf("El monitor ya está en uso.\n"); // Imprime un mensaje indicando que
                                            // el monitor ya está en uso
  }
}

void stop_monitor() {
  if (monitor_pid != -1) // Verifica si el monitor está en ejecución
  {
    kill(monitor_pid, SIGTERM); // Envía una señal SIGTERM al proceso del
                                // monitor para detenerlo
    waitpid(monitor_pid, NULL,
            0);       // Espera a que el proceso del monitor termine
    monitor_pid = -1; // Resetea el PID del monitor a -1 indicando que no está
                      // en ejecución
    monitoring = 0;   // Marca que el monitor no está en ejecución
    printf("Monitor detenido.\n"); // Imprime un mensaje indicando que el
                                   // monitor ha sido detenido
  } else                           // Si el monitor no está en ejecución
  {
    printf("El monitor no está en ejecución.\n"); // Imprime un mensaje
                                                  // indicando que el monitor no
                                                  // está en ejecución
  }
}

void status_monitor() {
  const char *fifo_path =
      "/tmp/monitor_fifo"; // Ruta del FIFO para comunicación entre procesos
  int option =
      0; // Variable para almacenar la opción seleccionada por el usuario
  printf("Monitoreo en ejecución (PID: %d).\n\n",
         getpid()); // Imprime el PID del proceso actual

  // Menú interactivo
  printf("Selecciona la métrica que deseas ver:\n");
  printf("    1. Uso de CPU\n");
  printf("    2. Uso de Memoria\n");
  printf("    3. Uso de Disco\n");
  printf("    4. Uso de Red\n");
  printf("    5. Número de Procesos\n");
  printf("    6. Cambios de Contexto\n");
  printf("    7. Todas las métricas\n");
  printf("    8. Salir\n");
  printf("    Selecciona una opción (1-8): ");
  // Lee la opción seleccionada por el usuario
  if (scanf("%d", &option) != 1) {
    fprintf(stderr, "Error reading input\n");
  }
  printf("\n");

  if (option == 8) {
    return; // Salir si la opción es 8
  }

  // Crear el FIFO si no existe
  if (mkfifo(fifo_path, 0666) == -1 && errno != EEXIST) {
    perror("Error al crear el FIFO");
    return;
  }

  // Abrir el FIFO en modo de lectura
  int fifo_fd = open(fifo_path, O_RDONLY); // Abre el FIFO para leer datos
  if (fifo_fd == -1) // Si ocurre un error al abrir el FIFO
  {
    perror("Error al abrir el FIFO"); // Imprime un mensaje de error
    return;                           // Sale de la función
  }

  // Intentar leer datos solo una vez
  char buffer[1024]; // Buffer para almacenar los datos leídos del FIFO
  memset(buffer, 0, sizeof(buffer)); // Limpiar el buffer
  ssize_t bytes_read =
      read(fifo_fd, buffer, sizeof(buffer) - 1); // Lee datos del FIFO
  if (bytes_read > 0)                            // Si se leyeron datos
  {
    buffer[bytes_read] = '\0'; // Asegurar que el buffer sea una cadena válida
    // Analizar el JSON recibido
    cJSON *json = cJSON_Parse(buffer); // Parsea el JSON recibido
    if (json)                          // Si el JSON fue parseado correctamente
    {
      // Obtener los valores de las métricas
      cJSON *cpu_usage =
          cJSON_GetObjectItem(json, "cpu_usage"); // Obtiene el uso de CPU
      cJSON *memory_usage = cJSON_GetObjectItem(
          json, "memory_usage"); // Obtiene el uso de memoria
      cJSON *disk_usage =
          cJSON_GetObjectItem(json, "disk_usage"); // Obtiene el uso de disco
      cJSON *network_usage =
          cJSON_GetObjectItem(json, "network_usage"); // Obtiene el uso de red
      cJSON *process_count = cJSON_GetObjectItem(
          json, "process_count"); // Obtiene el número de procesos
      cJSON *context_switches = cJSON_GetObjectItem(
          json, "context_switches"); // Obtiene las interrupciones de contexto

      // Mostrar valores según la opción seleccionada
      if (option == 1 &&
          cpu_usage) // Si la opción es 1 y se obtuvo el uso de CPU
      {
        printf("Uso de CPU: %.2f%%\n\n",
               cpu_usage->valuedouble); // Imprime el uso de CPU
      }
      if (option == 2 &&
          memory_usage) // Si la opción es 2 y se obtuvo el uso de memoria
      {
        printf("Uso de Memoria: %.2f MB\n\n",
               memory_usage->valuedouble); // Imprime el uso de memoria
      }
      if (option == 3 &&
          disk_usage) // Si la opción es 3 y se obtuvo el uso de disco
      {
        printf("Uso de Disco: %.2f GB\n\n",
               disk_usage->valuedouble); // Imprime el uso de disco
      }
      if (option == 4 &&
          network_usage) // Si la opción es 4 y se obtuvo el uso de red
      {
        printf("Uso de Red: %.2f KB/s\n\n",
               network_usage->valuedouble); // Imprime el uso de red
      }
      if (option == 5 &&
          process_count) // Si la opción es 5 y se obtuvo el número de procesos
      {
        printf("Número de Procesos: %d\n\n",
               process_count->valueint); // Imprime el número de procesos
      }
      if (option == 6 && context_switches) // Si la opción es 6 y se obtuvieron
                                           // las interrupciones de contexto
      {
        printf("Cambios de Contexto: %d\n\n",
               context_switches
                   ->valueint); // Imprime las interrupciones de contexto
      }
      if (option == 7) // Si la opción es 7, mostrar todas las métricas
      {
        if (cpu_usage) {
          printf("Uso de CPU: %.2f%%\n", cpu_usage->valuedouble);
        }
        if (memory_usage) {
          printf("Uso de Memoria: %.2f MB\n", memory_usage->valuedouble);
        }
        if (disk_usage) {
          printf("Uso de Disco: %.2f GB\n", disk_usage->valuedouble);
        }
        if (network_usage) {
          printf("Uso de Red: %.2f KB/s\n", network_usage->valuedouble);
        }
        if (process_count) {
          printf("Número de Procesos: %d\n", process_count->valueint);
        }
        if (context_switches) {
          printf("Cambios de Contexto: %d\n", context_switches->valueint);
        }
        printf("\n");
      }
      cJSON_Delete(json);
    } else {
      fprintf(stderr, "Error al parsear el JSON recibido.\n");
    }
  } else {
    fprintf(stderr, "Error al leer datos del FIFO.\n");
  }
  close(fifo_fd); // Cierra el descriptor de archivo del FIFO
}
