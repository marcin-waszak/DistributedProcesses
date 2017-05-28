//
// Created by marcin on 28.05.17.
//

#ifndef PROMAN_LOG_H
#define PROMAN_LOG_H

#include <utility>
#include <cstdio>

class Log {
public:
  template <typename ...Args>
  static void Out(Args&&... args) {
    char buffer_[256];
    snprintf(buffer_, sizeof(buffer_), std::forward<Args>(args)...);
    fprintf(stdout, "%s\n", buffer_);
    fflush(stdout);
  }

  template <typename ...Args>
  static void Info(Args&&... args) {
    if (Enable) {
      char buffer_[256];
      snprintf(buffer_, sizeof(buffer_), std::forward<Args>(args)...);
      fprintf(stdout, "%s\n", buffer_);
      fflush(stdout);
    }
  }

  template <typename ...Args>
  static void Error(Args&&... args) {
    if (Enable) {
      char buffer_[256];
      snprintf(buffer_, sizeof(buffer_), std::forward<Args>(args)...);
      fprintf(stderr, "%s\n", buffer_);
    }
  }

  template <typename ...Args>
  static void PutError(Args&&... args) {
    if (Enable)
      perror(std::forward<Args>(args)...);
  }

  static bool Enable;
};

#endif //PROMAN_LOG_H
