#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include "nodes.h"

#define IN
#define OUT // parameters used to output value

typedef unsigned char vm_status;

#define VM_OK  (vm_status) 1 //
#define VM_OOM (vm_status) 2 // out of memory
#define VM_ERR (vm_status) 3

/**
 * Initializes MAX_MEM bytes to be used by `code_malloc`.
 */
void code_malloc_init();

/**
 * Used memory for code
 */
size_t code_length();

vm_status load_expressions(OUT struct ExpressionList *result);

/**
 * Loads expression from stdin.
 * @param result the loaded expression
 * @return 0 OK
 *
 */
vm_status load_expression(OUT struct Expression **result);

/**
 * Prints value in a human-readable form.
 */
void print_value(FILE *output, struct Expression *value);
#endif
