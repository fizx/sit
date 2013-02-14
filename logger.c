#include <stdarg.h>
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

FILE *logfile;

void
sit_log(const char *level, const char *format, ...) {
	va_list args;
  va_start(args, format);
  if(logfile) {
    fprintf(logfile, "%s ", level);
    vfprintf(logfile, format, args);
    fprintf(logfile, "\n");
  }
}

void
set_logger(FILE *file) {
  logfile = file;
}