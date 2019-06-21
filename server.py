#!/usr/bin/env python3
import sys
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

  def audio_stream_open(self, sample_rate, buffer_size, device_index=0, input=False):
    if input:
      try:
        self.audio_stream = pa.open(format=pyaudio.paInt16,
                                    channels=2,
                                    rate=sample_rate,
                                    frames_per_buffer=buffer_size,
                                    input=True,
                                    input_device_index=device_index,
                                    as_loopback=True)
                                    #stream_callback=self.get_audio_chunk)
      except Exception as e:
        print(e)
        print("*** audio device doesn't support loopback")
        sys.exit(1)

    else:
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

  def audio_device_select(self, pa):
    if pa.get_default_input_device_info() == -1:
      print("*** no usable device found")
      sys.exit(1)

    # print WASAPI devices
    print("\naudio devices: ")
    for i in range(pa.get_device_count()):
      info = pa.get_device_info_by_index(i)
      #if info['hostApi'] != 1:
        #continue
      print(f" * {info['index']}: {info['name']} ({info['defaultSampleRate']})")

    userin = input("\nselect device: ")
    if not userin.isdigit():
      print("*** bad input")
      sys.exit(1)
    return int(userin)

  def audio_device_read(self):
    data = self.audio_stream.read(512)
    print(data)
    return data
    
if __name__ == "__main__":
  # init pyaudio
  print("** audio output init")
  pa = pyaudio.PyAudio()

  # init pipe server
  print("** pipe server init")
  pipe_server = PipeServer("127.0.0.1", 24325)

  audio_device_id = pipe_server.audio_device_select(pa)
  pipe_server.audio_stream_open(44100, 512, audio_device_id, input=True)
  rec = []
  for i in range(int(44100/512*5)):
    rec.append(pipe_server.audio_device_read())

  import wave
  wavefile = wave.open("test.wav", 'wb')
  wavefile.setnchannels(2)
  wavefile.setsampwidth(pa.get_sample_size(pyaudio.paInt16))
  wavefile.setframerate(44100)
  wavefile.writeframes(b"".join(rec))
  wavefile.close()
  sys.exit(0)

  # run socket thread
  pipe_server.running = True
  sock_thread = threading.Thread(target=pipe_server.run())
  sock_thread.start()
  while True:
    time.sleep(0.25)
