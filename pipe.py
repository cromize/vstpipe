#!/usr/bin/env python3
import time
import queue
import socket
import pyaudio

class PipeCommand:
  NONE_COMMAND = 0
  QUIT_COMMAND = 1
  AUDIO_PROCESS_COMMAND = 2

class PipeServer():
  def __init__(self, host, port, audio_device):
    self.host = host
    self.port = port
    self.client_buf = queue.Queue()
    self.buffer_size = 0
    self.audio_device = audio_device
    self.ready = False
    self.running = False
    self.input_mode = False

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

  def recvData(self, n):
    # recv all data
    buf = b""
    while len(buf) < n:
      buf += self.c.recv(n)
      if (len(buf) <= 0):
        return False
    return buf

  def get_audio_chunk(self, in_data, frame_count, time_info, status):
    if not self.client_buf.empty():
      return (self.client_buf.get(), pyaudio.paContinue)
    else:
      return (b"", pyaudio.paAbort)
      

  def process(self):
    # update buffer size from DAW
    buffer_size = int.from_bytes(self.recvData(4), "little")
    if buffer_size != self.buffer_size:
      print("*** buffer size", buffer_size, "samples")
      self.buffer_size = buffer_size
      #self.audio_device.audio_stream_reset()

      try:
        self.audio_device.audio_stream_close()
      except Exception:
        pass

      self.audio_device.audio_stream_open(44100, self.buffer_size,
                                          self.audio_device.info['index'],
                                          input=self.input_mode, audio_callback=self.get_audio_chunk)
      self.audio_device.audio_stream.start_stream()

    # audio in/out
    # TODO: handle this more intelligently
    if self.input_mode:
      # send windows input
      self.recvData(2*4*self.buffer_size)
      data = self.audio_device.audio_stream.read(self.buffer_size)
      self.c.sendall(data)
    else:
      # recv from VST
      try:
        self.client_buf.put(self.recvData(2*4*self.buffer_size))
      except Exception as e:
        print(e)
      self.c.send(b"")

  def do(self, command):
    if command == PipeCommand.NONE_COMMAND:
      self.ready = False
    elif command == PipeCommand.QUIT_COMMAND:
      self.ready = False
    elif command == PipeCommand.AUDIO_PROCESS_COMMAND:
      self.process()

