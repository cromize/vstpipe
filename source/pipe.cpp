#include <stdio.h>
#include <ws2tcpip.h>
#include <chrono>
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
  sendData(input, 2 * frames * sizeof(float));
  recvData(output, 2 * frames * sizeof(float));
}

void Pipe::connectPipe() {
  // init address struct
  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(4144);
  inet_pton(AF_INET, "127.0.0.1", &sa.sin_addr.s_addr);
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  // check socket
  if (sock < 0) {
    return;
  }

  if (connect(sock, (struct sockaddr*) &sa, sizeof(sa))) {
    return;
  }

  // set non-blocking
  ioctlsocket(sock, FIONBIO, 0);
}

void Pipe::disconnectPipe() {
  ready = false;
  if (sock >= 0 && closesocket(sock)) {
    // fail
  }
  sock = -1;
}

bool Pipe::sendData(void *data, int n) {
  if (n <= 0) return false;
  send(sock, (char *) data, n, 0);
  return true;
}

bool Pipe::recvData(void *data, int n) {
  if (n <= 0) return false;
  recv(sock, (char *)data, n, 0);
  return true;
}

bool Pipe::isReady() {
  return ready;
}
