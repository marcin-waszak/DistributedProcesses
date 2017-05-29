//
// Created by marcin on 28.05.17.
//

#ifndef PROMAN_LOG_H
#define PROMAN_LOG_H

#include <utility>
#include <cstdio>
#include <cstdarg>

class Log {
public:
  static void Out(const char* fmt, ...)
  {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);

    fprintf(stdout, "%s\n", buffer);
    fflush(stdout);
  }

  static void Info(const char* fmt, ...)
  {
    if(Enable) {
      char buffer[256];
      va_list args;
      va_start(args, fmt);
      vsprintf(buffer, fmt, args);
      va_end(args);

      fprintf(stdout, "%s\n", buffer);
      fflush(stdout);
    }
  }

  static void Error(const char* fmt, ...)
  {
    if(Enable) {
      char buffer[256];
      va_list args;
      va_start(args, fmt);
      vsprintf(buffer, fmt, args);
      va_end(args);

      fprintf(stderr, "%s\n", buffer);
    }
  }

  static void PutError(const char* x) {
    if (Enable)
      perror(x);
  }

  static bool Enable;
};

#endif //PROMAN_LOG_H
