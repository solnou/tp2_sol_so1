enable_testing()

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src) # CMAKE_SOURCE_DIR apunta al directorio raíz del proyecto
set(TEST_DIR ${CMAKE_CURRENT_LIST_DIR}) # CMAKE_CURRENT_LIST_DIR apunta al directorio actual donde está este CMakeLists.txt

add_executable(${PROJECT_NAME}_unit_tests
    ${TEST_DIR}/test_internos.c
    ${SRC_DIR}/commands.c
    ${SRC_DIR}/executions.c
    ${SRC_DIR}/monitor.c
    ${SRC_DIR}/shell.c
)

set_target_properties(${PROJECT_NAME}_unit_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

target_link_libraries(${PROJECT_NAME}_unit_tests PRIVATE unity::unity cjson::cjson)

add_test(NAME ${PROJECT_NAME}_UnitTests COMMAND ${CMAKE_BINARY_DIR}/tests/${PROJECT_NAME}_unit_tests)

set_tests_properties(${PROJECT_NAME}_UnitTests PROPERTIES WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
