// Definition of the nodes loaded up from the file

// TYPES

#define T_COMPOUND_EXPRESSION '('
#define T_VARIABLE 'v'
#define T_STRING 's'
#define T_INT32 'i'

#include <stdint.h>

struct string16 {
    int16_t length;
    char *value;
};

struct Expression {
      char type;
      union {
         int32_t iVal;
         struct string16 s16Val;
         struct Expression *exprsVal;
      } value;
};
