#include "loader.h"
#include "log.h"

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
        PRINT_SIZE(struct string16);
        PRINT_SIZE(int32_t);
        PRINT_SIZE(int16_t);
        PRINT_SIZE(char);
        PRINT_SIZE(char*);
        PRINT_SIZE(struct Expression*);
        PRINT_SIZE(struct Expression);
        PRINT_SIZE(struct ExpressionList*);
        PRINT_SIZE(struct ExpressionList);
        PRINT_SIZE(value_t);
        */

        return load_result;
    }
}
