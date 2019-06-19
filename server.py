#!/usr/bin/env python3
import time
import queue
import socket
import signal
import pyaudio
import threading

# drop python KeyboardInterrupt handle
signal.signal(signal.SIGINT, signal.SIG_DFL)

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
    self.audio_stream = None
    self.ready = False
    self.running = False

  def run(self):
    while self.running:
      self.listen()
      print("** pipe closed")
      self.flush()
      time.sleep(0.25)

  def listen(self):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
      s.bind((self.host, self.port))
      s.listen(1)
      self.c, addr = s.accept()
      with self.c:
        print("\n** pipe connected", addr)
        self.ready = True
        while self.ready:
          # accept command from client
          command = self.recvData(1)
          if command == None:
            continue
          command = int.from_bytes(command, "little")
          self.do(command)

  def flush(self):
    self.client_buf.queue.clear()
    self.buffer_size = 0

  def get_audio_chunk(self, in_data, frame_count, time_info, status):
    return (self.client_buf.get(), pyaudio.paContinue)

  def recvData(self, n):
    # recv all data
    remaining = 0
    buf = b""
    while len(buf) < n:
      buf += self.c.recv(n)
      if (len(buf) <= 0):
        return False
    return buf

  def process(self):
    # update buffer size from DAW
    buffer_size = int.from_bytes(self.recvData(4), "little")
    if buffer_size != self.buffer_size:
      print("*** buffer size", buffer_size, "samples")
      self.buffer_size = buffer_size
      self.audio_stream_reset()

    # audio in/out
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
    self.audio_stream = pa.open(format=pyaudio.paFloat32,
                                channels=2,
                                rate=sample_rate,
                                frames_per_buffer=buffer_size,
                                output=True,
                                stream_callback=self.get_audio_chunk)

  def audio_stream_close(self):
    self.audio_stream.close()

  def audio_stream_reset(self):
    try:
      self.audio_stream_close()
    except Exception:
      pass

    self.audio_stream_open(44100, self.buffer_size)
    self.audio_stream.start_stream()
    
if __name__ == "__main__":
  # init pyaudio
  print("** audio output init")

  # init pipe server
  print("** pipe server init")
  pipe_server = PipeServer("127.0.0.1", 24325)

  # init audio output
  pa = pyaudio.PyAudio()

  # run socket thread
  pipe_server.running = True
  sock_thread = threading.Thread(target=pipe_server.run())
  sock_thread.start()
  while True:
    time.sleep(0.25)
