#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdarg.h>

void log_error(const char *format, ...);
void log_load_error(const char *expected, FILE *source);
void log_trace(const char *format, ...);

#endif
