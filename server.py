#!/usr/bin/env python3
import socket
import pyaudio

class PipeServer():
  def __init__(self, host, port):
    self.host = host
    self.port = port

  def listen(self):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.bind((self.host, self.port))
      s.listen(1)
      c, addr = s.accept()
      with c:
        print("** pipe connected", addr)
        while True:
          c.send(b"lolo")
          print(c.recv(4))
          #print(c.recv(2*4*1024))
    
if __name__ == "__main__":
  # init pyaudio
  print("** audio output init")
  pa = pyaudio.PyAudio()

  # init pipe server
  print("** pipe server init")
  pipe_server = PipeServer("127.0.0.1", 4144)
  pipe_server.listen()
  
