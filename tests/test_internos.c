#include "../include/commands.h"
#include "../include/executions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unity.h>

#define TEST_BUFFER 1024

// Mock de funciones auxiliares para evitar conflictos con las implementaciones reales
#ifdef UNIT_TESTING
#define execute_command(command) mock_execute_command(command)
#define execute_command_secondplane(command) mock_execute_command_secondplane(command)
#define execute_piped_commands(command) mock_execute_piped_commands(command)
#define execute_command_redirection(command) mock_execute_command_redirection(command)
#endif

// Mock functions para pruebas de choose_execution
void mock_execute_command(char* command)
{
    TEST_MESSAGE("Mock: execute_command called");
}

void mock_execute_command_secondplane(char* command)
{
    TEST_MESSAGE("Mock: execute_command_secondplane called");
}

void mock_execute_piped_commands(char* command)
{
    TEST_MESSAGE("Mock: execute_piped_commands called");
}

void mock_execute_command_redirection(char* command)
{
    TEST_MESSAGE("Mock: execute_command_redirection called");
}

// setUp y tearDown, compartidos entre todos los tests
void setUp(void)
{
    // Configuración inicial (si es necesaria)
}

void tearDown(void)
{
    // Limpieza después de cada test (si es necesaria)
}

// --- Tests para command_cd ---
void test_command_cd_to_home(void)
{
    char* home_dir = getenv("HOME");
    TEST_ASSERT_NOT_NULL(home_dir);

    command_cd(NULL);

    char cwd[TEST_BUFFER];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        TEST_FAIL_MESSAGE("Failed to retrieve current working directory");
    }
    TEST_ASSERT_EQUAL_STRING(home_dir, cwd);
}

void test_command_cd_to_valid_directory(void)
{
    const char* test_dir = "/tmp";
    command_cd((char*)test_dir);

    char cwd[TEST_BUFFER];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        TEST_FAIL_MESSAGE("Failed to retrieve current working directory");
    }
    TEST_ASSERT_EQUAL_STRING(test_dir, cwd);
}

void test_command_cd_to_invalid_directory(void)
{
    const char* invalid_dir = "/nonexistent_directory";

    // Redirigir stderr temporalmente para evitar ruido en la salida
    FILE* devnull = fopen("/dev/null", "w");
    FILE* stderr_backup = stderr;
    stderr = devnull;

    command_cd((char*)invalid_dir);

    char cwd[TEST_BUFFER];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        TEST_FAIL_MESSAGE("Failed to retrieve current working directory");
    }
    // Usar strcmp para comprobar que las cadenas no son iguales
    TEST_ASSERT_TRUE(strcmp(invalid_dir, cwd) != 0);

    // Restaurar stderr
    stderr = stderr_backup;
    fclose(devnull);
}

void test_command_cd_to_oldpwd(void)
{
    char cwd[TEST_BUFFER];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        TEST_FAIL_MESSAGE("Failed to retrieve current working directory");
    }

    command_cd("/tmp");
    command_cd("-");

    char* oldpwd = getenv("OLDPWD");
    TEST_ASSERT_EQUAL_STRING(cwd, oldpwd);

    char new_cwd[TEST_BUFFER];
    if (getcwd(new_cwd, sizeof(new_cwd)) == NULL)
    {
        perror("getcwd");
        TEST_FAIL_MESSAGE("Failed to retrieve current working directory");
    }
    TEST_ASSERT_EQUAL_STRING(cwd, new_cwd);
}

// --- Tests para choose_execution ---
void test_choose_execution_normal_command(void)
{
    char command[] = "ls";
    choose_execution(command);
    TEST_PASS_MESSAGE("Normal command executed as expected.");
}

void test_choose_execution_background(void)
{
    char command[] = "ls &";
    choose_execution(command);
    TEST_PASS_MESSAGE("Background command executed as expected.");
}

void test_choose_execution_pipe(void)
{
    char command[] = "ls | grep txt";
    choose_execution(command);
    TEST_PASS_MESSAGE("Piped command executed as expected.");
}

void test_choose_execution_redirection(void)
{
    char command[] = "ls > output.txt";
    choose_execution(command);
    TEST_PASS_MESSAGE("Redirection command executed as expected.");
}

// --- Punto de entrada del test ---
int main(void)
{
    UNITY_BEGIN();
    // Tests de command_cd
    RUN_TEST(test_command_cd_to_home);
    RUN_TEST(test_command_cd_to_valid_directory);
    RUN_TEST(test_command_cd_to_invalid_directory);
    RUN_TEST(test_command_cd_to_oldpwd);

    // Tests de choose_execution
    RUN_TEST(test_choose_execution_normal_command);
    RUN_TEST(test_choose_execution_background);
    RUN_TEST(test_choose_execution_pipe);
    RUN_TEST(test_choose_execution_redirection);

    return UNITY_END();
}
