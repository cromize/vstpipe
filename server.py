#!/usr/bin/env python3
import socket
import pyaudio
import time
import queue

class PipeCommand:
  NONE_COMMAND = 0
  QUIT_COMMAND = 1
  AUDIO_PROCESS_COMMAND = 2

class PipeServer():
  def __init__(self, host, port):
    self.host = host
    self.port = port
    self.client_buf = queue.Queue()
    self.buffer_size = 0
    self.sock = None
    self.audio_stream = None
    self.is_audio_stream_open = False
    self.ready = False

  def listen(self, pa):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.bind((self.host, self.port))
      s.listen(1)
      self.c, addr = s.accept()
      with self.c:
        print("** pipe connected", addr)
        self.ready = True
        while self.ready:
          command = self.recvData(1)
          if command == None:
            continue
          command = int.from_bytes(command, "little")
          self.do(command)
          if not self.audio_stream_open:
            self.audio_stream_open(44100, self.buffer_size)
            self.audio_stream.start_stream()
            self.is_audio_stream_open = True

  def get_audio_chunk(self, in_data, frame_count, time_info, status):
    return (self.client_buf.get(), pyaudio.paContinue)

  def recvData(self, n):
    remaining = 0
    buf = b""
    while len(buf) < n:
      buf += self.c.recv(n)
      if (len(buf) <= 0):
        return False
    return buf

  def process(self):
    buffer_size = int.from_bytes(self.recvData(4), "little")
    if buffer_size != self.buffer_size:
      print(buffer_size)
      try:
        self.audio_stream.stop_stream()
        self.audio_stream_close()
      except Exception:
        pass

      self.audio_stream_open(44100, buffer_size)
      self.audio_stream.start_stream()
      self.buffer_size = buffer_size
      self.is_audio_stream_open = True

    #self.client_buf = self.recvData(2*4*self.buffer_size)
    self.client_buf.put(self.recvData(2*4*self.buffer_size))
    self.c.send(b"")

  def do(self, command):
    if command == PipeCommand.NONE_COMMAND:
      self.ready = False
    elif command == PipeCommand.QUIT_COMMAND:
      self.ready = False
    elif command == PipeCommand.AUDIO_PROCESS_COMMAND:
      self.process()

  def audio_stream_open(self, sample_rate, buffer_size):
    print("** audio stream open")
    self.audio_stream = pa.open(format=pyaudio.paFloat32,
                                channels=2,
                                rate=sample_rate,
                                frames_per_buffer=buffer_size,
                                output=True,
                                stream_callback=self.get_audio_chunk)

  def audio_stream_close(self):
    print("** audio stream close")
    self.audio_stream.close()
    
if __name__ == "__main__":
  # init pyaudio
  print("** audio output init")

  # init pipe server
  print("** pipe server init")
  pipe_server = PipeServer("127.0.0.1", 24325)

  # init audio output
  pa = pyaudio.PyAudio()

  while True:
    # listen
    pipe_server.listen(pa)

    print("** pipe closed")
    time.sleep(1)
    
