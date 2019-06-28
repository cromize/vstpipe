#include <chrono>
#include <stdio.h>
#include <ws2tcpip.h>
#include "pipe.h"

#pragma comment(lib, "Ws2_32.lib")

Pipe::Pipe() {
  sock = -1;
  running = true;
  ready = false;
  main_thread = std::thread(&Pipe::run, this);
}

Pipe::~Pipe() {
  running = false;
  main_thread.join();
}

void Pipe::init() {
  WSADATA wsaData;
  if (WSAStartup(0x202, &wsaData)) {
    return;
  }
}

void Pipe::run() {
  init();
  while (running) {
    // connect
    std::this_thread::sleep_for(std::chrono::duration<double>(0.25));
    connectPipe();

    if (sock < 0) {
      continue;
    }

    // wait for server disconnect 
    ready = true;
    while (running && ready) {
      std::this_thread::sleep_for(std::chrono::duration<double>(0.25));
    }
    disconnectPipe();
  }
}

void Pipe::process(float *input, float *output, int frames) {
  if (!ready) return;

  // send buffer size
  sendData<uint32_t>(frames);

  sendData(input, 2 * frames * sizeof(float));
  recvData(output, 2 * frames * sizeof(float));
}

void Pipe::connectPipe() {
  // init address struct
  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(PIPE_PORT);
  inet_pton(AF_INET, PIPE_HOST, &sa.sin_addr.s_addr);
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  // Nagle off
  setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)1, sizeof(int));

  // check socket
  if (sock < 0) {
    return;
  }

  if (connect(sock, (struct sockaddr*) &sa, sizeof(sa))) {
    return;
  }

  // set non-blocking
  //ioctlsocket(sock, FIONBIO, 0);
}

void Pipe::disconnectPipe() {
  ready = false;
  if (sock >= 0) {
    sendData<uint8_t>(PipeCommand::QUIT_COMMAND);
    shutdown(sock, SD_SEND);
    if (closesocket(sock)) {
      // fail
    }
  }
  sock = -1;
}

bool Pipe::sendData(void *data, int n) {
  if (n <= 0) return false;

  int remaining = 0;
  while (remaining < n) {
    int actual = send(sock, (char *) data, n, 0);
    if (actual <= 0) {
      ready = false;
      return false;
    }
    remaining += actual;
  }
  return true;
}

bool Pipe::recvData(void *data, int n) {
  if (n <= 0) return false;

  int remaining = 0;
  while (remaining < n) {
    int actual = recv(sock, (char *)data + remaining, n - remaining, 0);
    if (actual <= 0) {
      ready = false;
      return false;
    }
    remaining += actual;
  }
  return true;
}

bool Pipe::isReady() {
  return ready;
}

void Pipe::flush() {
  setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)1, sizeof(int));
  setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)0, sizeof(int));
}
