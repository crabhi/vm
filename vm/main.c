#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include "nodes.h"
#include "main.h"

#define PRINT_SIZE(x) printf("sizeof(" #x ") = %lu\n", sizeof(x))

#define MAX_MEM (1024)

/**
 * Like fprintf, but adds "\n".
 */
void vfprintfln(FILE *file, const char *format, va_list args) {
    vfprintf(file, format, args);
    fprintf(file, "\n");
}

void log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);

    vfprintfln(stderr, format, args);

    va_end(args);
}

void log_load_error(const char *expected, FILE *source) {
    if (feof(source)) {
        log_error("Unexpected end of file. Expected %s.", expected);
    } else if (ferror(source)) {
        log_error("Error reading input");
    }
}

void log_trace(const char *format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(stdout, "[trace] ");
    vfprintfln(stdout, format, args);

    va_end(args);
}

/**
 * Loads expression from stdin.
 * @return 0 OK
 *
 */
vm_status load_expression(struct Expression *result, const size_t max_length, size_t *used) {
    *used = 0;
    FILE *source = stdin;

    struct Expression *current_expr = result;

    int c;
    while (EOF != (c = getc(source))) {
        if (max_length - *used < sizeof(struct Expression)) {
            log_error("Out of memory, used %lu, maximum %lu", *used, max_length);
            return VM_OOM;
        }

        *used += sizeof(struct Expression);
        current_expr->type = (char) c;

        switch (c) {
            case T_INT32: {
                int32_t val;
                size_t value_size = sizeof(val);
                size_t nmemb = 1;

                log_trace("Want to read %lu*%lu", nmemb, value_size);

                size_t size_read = fread(&val, value_size, nmemb, source);

                log_trace("Read %lu: %i", size_read, val);

                if (size_read < nmemb) {
                    log_load_error("<int32>", source);
                    return VM_ERR;
                } else {
                    current_expr->value.iVal = val;
                    current_expr++;
                    break;
                }
                assert (0); // Should be unreachable.
            }
            case T_STRING_16: {
                struct string16 *str = &(current_expr->value.s16Val);
                if(!fread(&(str->length), sizeof str->length, 1, source)) {
                    log_load_error("<string16>.length", source);
                    return VM_ERR;
                } else {
                    current_expr++;
                    str->value = (unsigned char*) current_expr;
                    log_trace("expected length: %i * %lu", str->length, sizeof *str->value);
                    if (str->length > fread(str->value, sizeof *str->value, str->length, source)) {
                        log_load_error("<string16>.value", source);
                        return VM_ERR;
                    } else {
                        *used += str->length;
                        current_expr = (struct Expression*) (str->value + str->length);
                        return VM_OK;
                    }
                }

                break;
            }
            case '\n':
            case ' ':
            case '\t':
                log_trace("Ignored character: 0x%x", c);
                break;
            default:
                log_error("Loading type %c not implemented", c);
                return VM_ERR;
        }
    }

    return VM_OK;
}

void print_string(FILE *output, struct string16 *value) {
    for (int i = 0; i < value->length; i++) {
        fputc(value->value[i], output);
    }
}

void print_value(FILE *output, struct Expression *value) {
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

int main(void) {
    // Runtime init checks

    if (sizeof(int32_t) != 4) {
        printf("Unexpected size of int32_t: %lu\n", sizeof(int32_t));
        return 201;
    }

    struct Expression *code = (struct Expression*) malloc(MAX_MEM);
    size_t code_length = 0;

    const vm_status load_result = load_expression(code, MAX_MEM, &code_length);

    assert(code_length <= MAX_MEM);

    log_trace("Total code size: %lu bytes.", code_length);

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
