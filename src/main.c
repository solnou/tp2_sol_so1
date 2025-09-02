#include "../include/commands.h"
#include "../include/executions.h"
#include "../include/monitor.h"
#include "../include/shell.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  init_shell(argc, argv);
  while (1) {
    sleep(1);
  }
  return 0;
}
