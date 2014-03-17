#include "log.h"

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
    } else {
        log_error("Expected %s.", expected);
    }
}

void log_trace(const char *format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(stdout, "[trace] ");
    vfprintfln(stdout, format, args);

    va_end(args);
}
