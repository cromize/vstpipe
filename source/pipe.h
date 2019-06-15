#ifndef PIPE_H
#define PIPE_H

#include <thread>
#pragma once

class Pipe {
public:
  Pipe();
  ~Pipe();

  void init();
  void run();
  void connectPipe();
  void disconnectPipe();
  bool sendData(void *data, int n);
  bool recvData(void *data, int n);
  bool isReady();

protected:
  bool running;
  bool ready;
  char pipe_name[32];
  std::thread main_thread;
  SOCKET sock;
};

#endif