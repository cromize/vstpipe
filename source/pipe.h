#ifndef PIPE_H
#define PIPE_H

#include "windows.h"
#pragma once

class Pipe {
public:
  Pipe();
  ~Pipe();

  void init();
  void connectPipe();
  void disconnectPipe();
  bool sendData(void *data, int n);
  bool recvData(void *data, int n);
  SOCKET getSock();

private:
  int id;
  char pipe_name[32];
  SOCKET sock;
};

#endif