#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <stdio.h>
#include <string.h>

#define WARN(x, ...)   sit_log("WARN", x,  ##__VA_ARGS__)
#define INFO(x, ...)   sit_log("INFO", x,  ##__VA_ARGS__)
#define ERR(x, ...)    sit_log("ERROR", x,  ##__VA_ARGS__)
#define DEBUG(x, ...)  if(isTestMode()){ sit_log("DEBUG", x,  ##__VA_ARGS__);}
#define PERROR(x)    sit_log("ERROR", "%s: %s", x, strerror(errno))

void
sit_log(const char *level, const char *format, ...);

void
set_logger(FILE *file);

#endif