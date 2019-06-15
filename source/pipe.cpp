#include <stdio.h>
#include <ws2tcpip.h>
#include "pipe.h"

#pragma comment(lib, "Ws2_32.lib")

Pipe::Pipe(int id) {
  sock = -1;
}

Pipe::~Pipe() {
  disconnectPipe();
}

void Pipe::init() {
  WSADATA wsaData;

  if (WSAStartup(0x202, &wsaData)) {
    // fail
    return;
  }
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
    // fail
    return;
  }

  if (connect(sock, (struct sockaddr*) &sa, sizeof(sa))) {
    // fail
    return;
  }

  // set non-blocking
  ioctlsocket(sock, FIONBIO, 0);
}

void Pipe::disconnectPipe() {
  if (sock >= 0 && closesocket(sock)) {
    // fail
  }
  sock = -1;
}

void Pipe::sendData(void *data, int n) {
  send(sock, (const char*) data, n, 0);
}

SOCKET Pipe::getSock() {
  return sock;
}
