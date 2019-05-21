#ifndef PIPE_H
#define PIPE_H

#include "windows.h"
#pragma once

class Pipe {
public:
  Pipe(int id);
  ~Pipe();

  void init();
  void check_broken_pipe();
  void send_data(void *data, int n);
  HANDLE get_pipe();

private:
  int id;
  char pipe_name[32];
  HANDLE pipe;
};

#endif