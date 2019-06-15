#ifndef PIPE_H
#define PIPE_H

#include "windows.h"
#pragma once

class Pipe {
public:
  Pipe(int id);
  ~Pipe();

  void init();
  void connectPipe();
  void disconnectPipe();
  void sendData(void *data, int n);
  SOCKET getSock();

private:
  int id;
  char pipe_name[32];
  SOCKET sock;
};

#endif