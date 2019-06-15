#!/usr/bin/env python3
import socket
import pyaudio


class PipeServer():
  def __init__(self, host, port):
    self.host = host
    self.port = port
    self.client_buf = b""

  def listen(self, pa):
    i = 0
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
          self.client_buf = c.recv(2*4*1024)
          c.send(b"")

  def send(self, data, n):
    if n <= 0:
      return False

  def get_audio_chunk(self, in_data, frame_count, time_info, status):
    return (self.client_buf, pyaudio.paContinue)
    
if __name__ == "__main__":
  # init pyaudio
  print("** audio output init")

  # init pipe server
  print("** pipe server init")
  pipe_server = PipeServer("127.0.0.1", 4144)

  # open output audio stream
  pa = pyaudio.PyAudio()

  # listen
  pipe_server.listen(pa)
  
