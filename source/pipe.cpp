#include <stdio.h>
#include "windows.h"
#include "pipe.h"

Pipe::Pipe(int id) {
  this->id = id;
}

Pipe::~Pipe() {
  DisconnectNamedPipe(pipe);
}

void Pipe::init() {
  pipe_name[32] = {};
  snprintf(pipe_name, 32, "\\\\.\\pipe\\vstpipe%d", id);
  pipe = CreateNamedPipe(
    pipe_name,
    PIPE_ACCESS_DUPLEX,
    PIPE_TYPE_BYTE | PIPE_NOWAIT,
    PIPE_UNLIMITED_INSTANCES,
    0,
    0,
    1,
    NULL
  );
}

void Pipe::check_broken_pipe() {
  // if closed, disconnect pipe
  DWORD nRead, nTotal, nLeft;
  PeekNamedPipe(pipe, 0, 8192, &nRead, &nTotal, &nLeft);
  if (nRead > 0) {
    DisconnectNamedPipe(pipe);
    pipe = CreateNamedPipe(
      pipe_name,
      PIPE_ACCESS_DUPLEX,
      PIPE_TYPE_BYTE | PIPE_NOWAIT,
      PIPE_UNLIMITED_INSTANCES,
      0,
      0,
      1, 
      NULL 
    );
  }
}

void Pipe::send_data(void *data, int n) {
  LPDWORD numBytesWritten = 0;
  WriteFile(
    pipe,
    data,
    n,
    numBytesWritten,
    NULL // not using overlapped IO
  );
}

HANDLE Pipe::get_pipe() {
  return pipe;
}
