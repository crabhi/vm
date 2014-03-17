#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "nodes.h"
#include "main.h"
#include "log.h"

#define PRINT_SIZE(x) printf("sizeof(" #x ") = %lu\n", sizeof(x))

#define MAX_MEM (1024)

char *code_heap = NULL;
char *code_heap_end = NULL;

#define code_malloc_t(type) ((type*) code_malloc(sizeof(type)))

/**
 * Initializes MAX_MEM bytes to be used by `code_malloc`.
 */
void code_malloc_init()
{
    if (code_heap_end != code_heap) {
        free(code_heap);
        code_heap = NULL;
        code_heap_end = NULL;
    }

    code_heap = malloc(MAX_MEM);
    code_heap_end = code_heap;

    if (code_heap == NULL) {
        log_error("Error when initializing code memory");
        exit(VM_OOM);
    }
}

char * code_malloc(size_t bytes)
{
    if (MAX_MEM - code_length() < bytes) {
        log_error("Out of memory, used %lu, maximum %lu, wanted %lu.", code_length(), MAX_MEM, bytes);
        exit(VM_OOM);
    }

    char *place = code_heap_end;
    code_heap_end += bytes;

    return place;
}

size_t code_length()
{
    return code_heap_end - code_heap;
}

/**
 * Loads expression from stdin.
 * @param result the loaded expression
 * @return 0 OK
 *
 */
vm_status load_expression(OUT struct Expression **result)
{
    FILE *source = stdin;

    int c;
    while (EOF != (c = getc(source))) {
        *result = code_malloc_t(struct Expression);
        struct Expression *expr = *result;

        expr->type = (char) c;

        switch (c) {
            case T_INT32:
                assert(1); // NOP
                int32_t val;
                size_t value_size = sizeof(val);
                size_t nmemb = 1;

                log_trace("Want to read %lu*%lu", nmemb, value_size);

                size_t size_read = fread(&val, value_size, nmemb, source);

                log_trace("Read %lu: %i", size_read, val);

                if (size_read < nmemb) {
                    log_load_error("<int32>", source);
                    return VM_ERR;
                }

                expr->value.iVal = val;
                break;
            case T_STRING_16:
                assert(1); // NOP
                struct string16 *str = &(expr->value.s16Val);

                if(!fread(&(str->length), sizeof str->length, 1, source))
                {
                    log_load_error("<string16>.length", source);
                    return VM_ERR;
                }

                str->value = code_malloc(str->length);

                log_trace("expected length: %i * %lu", str->length, sizeof *str->value);
                if (str->length > fread(str->value, sizeof *str->value, str->length, source))
                {
                    log_load_error("<string16>.value", source);
                    return VM_ERR;
                }
                break;
            case '\n':
            case ' ':
            case '\t':
                log_trace("Ignored character: 0x%x", c);
            default:
                log_error("Loading type %c not implemented", c);
                return VM_ERR;
        }

    return VM_OK;
    }

    log_error("Unexpected end of file");
    return VM_ERR;
}

void print_string(FILE *output, struct string16 *value)
{
    for (int i = 0; i < value->length; i++) {
        fputc(value->value[i], output);
    }
}

void print_value(FILE *output, struct Expression *value)
{
    switch (value->type) {
        case T_INT32:
            fprintf(output, "%i", value->value.iVal);
            break;
        case T_STRING_16:
            print_string(output, (struct string16*) &(value->value.s16Val));
            break;
        default:
            log_error("Not implemented type: %c", value->type);
    }
}

int main(void)
{
    // Runtime init checks

    if (sizeof(int32_t) != 4) {
        printf("Unexpected size of int32_t: %lu\n", sizeof(int32_t));
        return 201;
    }

    code_malloc_init();

    struct Expression *code;

    const vm_status load_result = load_expression(&code);

    log_trace("Total code size: %lu bytes.", code_length());

    if (VM_OK == load_result) {
        print_value(stdout, code);
        printf("\n");
        return 0;
    } else {
        log_error("Failed to load code, err: %i", load_result);

        /*
        PRINT_SIZE(struct Expression);
        PRINT_SIZE(struct string16);
        PRINT_SIZE(int32_t);
        PRINT_SIZE(int16_t);
        PRINT_SIZE(char);
        PRINT_SIZE(char*);
        PRINT_SIZE(struct Expression*);
        PRINT_SIZE(value_t);
        */

        return load_result;
    }
}

