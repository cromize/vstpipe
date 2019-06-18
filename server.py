#!/usr/bin/env python3
import socket
import pyaudio
import time

class PipeCommand:
  NONE_COMMAND = 0
  QUIT_COMMAND = 1
  AUDIO_PROCESS_COMMAND = 2

class PipeServer():
  def __init__(self, host, port):
    self.host = host
    self.port = port
    self.client_buf = b""
    self.frames = 0
    self.ready = False
    self.sock = None

  def listen(self, pa):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.bind((self.host, self.port))
      s.listen(1)
      self.c, addr = s.accept()
      with self.c:
        stream = pa.open(format=pyaudio.paFloat32,
                         channels=2,
                         rate=44100,
                         frames_per_buffer=1024,
                         output=True,
                         stream_callback=pipe_server.get_audio_chunk)
        stream.start_stream()
        print("** pipe connected", addr)
        self.ready = True
        #self.c.setblocking(0)
        while self.ready:
          command = self.recvData(1)
          if command == None:
            continue
          command = int.from_bytes(command, "big")
          self.do(command)

  def get_audio_chunk(self, in_data, frame_count, time_info, status):
    return (self.client_buf, pyaudio.paContinue)

  def recvData(self, n):
    remaining = 0
    buf = b""
    while len(buf) < n:
      buf += self.c.recv(n)
      if (len(buf) <= 0):
        return False
    return buf

  def process(self):
    self.frames = int.from_bytes(self.recvData(4), "little")
    print(self.frames)

    self.client_buf = self.recvData(2*4*self.frames)
    #print("len:", len(self.client_buf))
    self.c.send(b"")

  def do(self, command):
    if command == PipeCommand.NONE_COMMAND:
      self.ready = False
    elif command == PipeCommand.QUIT_COMMAND:
      self.ready = False
    elif command == PipeCommand.AUDIO_PROCESS_COMMAND:
      self.process()
    
if __name__ == "__main__":
  # init pyaudio
  print("** audio output init")

  # init pipe server
  print("** pipe server init")
  pipe_server = PipeServer("127.0.0.1", 24325)

  # open output audio stream
  pa = pyaudio.PyAudio()

  while True:
    # listen
    pipe_server.listen(pa)

    print("** pipe closed")
    time.sleep(1)
    
