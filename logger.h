#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <stdio.h>

#define WARN(x, ...)   sit_log("[WARN]", x,  ##__VA_ARGS__)
#define INFO(x, ...)   sit_log("[INFO]", x,  ##__VA_ARGS__)
#define ERR(x, ...)    sit_log("[ERR]", x,  ##__VA_ARGS__)

void
sit_log(char *level, char *format, ...);

void
set_logger(FILE *file);

#endif