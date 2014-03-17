// Definition of the nodes loaded up from the file
#ifndef _NODES_H_
#define _NODES_H_

// TYPES

#define T_EXPRESSION_LIST '('
#define T_VARIABLE 'v'
#define T_STRING_16 's'
#define T_INT32 'i'

#include <stdint.h>

struct string16 {
    int16_t length;
    char *value;
};

struct ExpressionList {
    struct Expression *head;
    struct ExpressionList *next;
};

typedef union {
    int32_t iVal;
    struct string16 s16Val;
    struct ExpressionList *exprsVal;
} value_t;

struct Expression {
    char type;
    value_t value;
};

#endif
