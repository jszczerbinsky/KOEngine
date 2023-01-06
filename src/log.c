#include <stdarg.h>
#include <stdio.h>
#include <time.h>

void Log(const char *format, ...)
{
  time_t     t        = time(NULL);
  struct tm *timedate = localtime(&t);

  char timestr[30];
  strftime(timestr, 30, "%d %b %H:%M:%S ", timedate);
  printf("%s", timestr);

  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  printf("\n");
  fflush(stdout);
}
