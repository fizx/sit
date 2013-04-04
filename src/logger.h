#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <stdio.h>
#include <string.h>

// #define DEBUG(x, ...)
// #define WARN(x, ...) 
// #define INFO(x, ...) 
// #define ERR(x, ...)  
// #define PERROR(x)    

#define COMPILE_WITH_DEBUG

#define WARN(x, ...)   sit_log("WARN", x,  ##__VA_ARGS__)
#define INFO(x, ...)   sit_log("INFO", x,  ##__VA_ARGS__)
#define ERR(x, ...)    sit_log("ERROR", x,  ##__VA_ARGS__)
#ifdef COMPILE_WITH_DEBUG
#define DEBUG(x, ...)  if(isTestMode()){ sit_log("DEBUG", x,  ##__VA_ARGS__);}
#else
#define DEBUG(x, ...)
#endif
#define PERROR(x, ...)    do{                     \
  char *z;                                        \
  asprintf(&z, x, ##__VA_ARGS__);                     \
  sit_log("ERROR", "%s: %s", z, strerror(errno)); \
  free(z);                                        \
} while(0)

void
sit_log(const char *level, const char *format, ...);

void
set_logger(FILE *file);

#endif