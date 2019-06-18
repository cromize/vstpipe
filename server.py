#!/usr/bin/env python3
import socket
import pyaudio

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

  def listen(self, pa):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.bind((self.host, self.port))
      s.listen(1)
      c, addr = s.accept()
      with c:
        stream = pa.open(format=pyaudio.paFloat32,
                         channels=2,
                         rate=44100,
                         frames_per_buffer=1024,
                         output=True,
                         stream_callback=pipe_server.get_audio_chunk)
        stream.start_stream()
        print("** pipe connected", addr)
        while True:
          command = c.recv(1)
          if command == None:
            continue
          print(command)
          self.do(command)

  def get_audio_chunk(self, in_data, frame_count, time_info, status):
    return (self.client_buf, pyaudio.paContinue)

  def recvData(c, n):
    remaining = 0
    while remaining < n:
      actual = c.recv(n)
      if actual <= 0:
        return
      remaining += actual

  def process(self):
    self.frames = int(c.recv(4))

    self.client_buf = c.recv(2*4*self.frames)
    c.send(b"")

  def do(self, command):
    if command == PipeCommand.NONE_COMMAND:
      print("** none command")
    elif command == PipeCommand.QUIT_COMMAND:
      print("** quit command")
    elif command == PipeCommand.AUDIO_PROCESS_COMMAND:
      process()
      print("** audio process command")
    
if __name__ == "__main__":
  # init pyaudio
  print("** audio output init")

  # init pipe server
  print("** pipe server init")
  pipe_server = PipeServer("127.0.0.1", 24325)

  # open output audio stream
  pa = pyaudio.PyAudio()

  # listen
  pipe_server.listen(pa)
  
