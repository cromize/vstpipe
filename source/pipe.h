#ifndef PIPE_H
#define PIPE_H

#include <thread>
#pragma once

const char PIPE_HOST[] = "127.0.0.1";
const int PIPE_PORT = 24325;

enum PipeCommand {
  NONE_COMMAND = 0,
  QUIT_COMMAND,
  AUDIO_PROCESS_COMMAND
};

class Pipe {
public:
  Pipe();
  ~Pipe();

  void init();
  void run();
  void process(float *input, float *output, int frames);
  void connectPipe();
  void disconnectPipe();
  bool sendData(void *data, int n);
  bool recvData(void *data, int n);
  bool isReady();
  void flush();

  template <typename T>
  bool sendData(T x) {
    return sendData(&x, sizeof(x));
  }

  template <typename T>
  bool recvData(T x) {
    return recvData(&x, sizeof(x));
  }

private:
  bool running;
  bool ready;
  char pipe_name[32];
  std::thread main_thread;
  SOCKET sock;
};

#endif