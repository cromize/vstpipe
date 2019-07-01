#!/usr/bin/env python3
import sys
import time
import signal
import pyaudio
import argparse
import threading
from pipe import PipeServer

# TODO: address/port selection

class AudioDevice():
  def __init__(self):
    self.info = None

  def audio_stream_open(self, sample_rate, buffer_size, device_index=0, input=False, audio_callback=None):
    if input:
      try:
        self.audio_stream = pa.open(format=pyaudio.paFloat32,
                                    channels=2,
                                    rate=sample_rate,
                                    frames_per_buffer=buffer_size,
                                    input=True,
                                    input_device_index=device_index,
                                    as_loopback=True,
                                    stream_callback=audio_callback)
      except Exception as e:
        print(e)
        #print("*** audio device doesn't support loopback")
        sys.exit(1)

    else:
      self.audio_stream = pa.open(format=pyaudio.paFloat32,
                                  channels=2,
                                  rate=sample_rate,
                                  frames_per_buffer=buffer_size,
                                  output=True,
                                  stream_callback=audio_callback)
    self.sample_rate = sample_rate
    self.buffer_size = buffer_size

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
    # no device
    if pa.get_default_input_device_info() == -1:
      print("*** no usable device found")
      sys.exit(1)

    # print WASAPI devices
    print("\nrecordable audio devices: ")
    for i in range(pa.get_device_count()):
      info = pa.get_device_info_by_index(i)
      if info['hostApi'] != 1:
        continue
      print(f" * {info['index']}: {info['name']} (sample rate: {int(info['defaultSampleRate'])} kHz)")

    # select device
    userin = input("\nselect device: ")
    if not userin.isdigit():
      print("*** bad input")
      sys.exit(1)

    # save selected
    self.info = pa.get_device_info_by_index(int(userin))

  def audio_device_read(self):
    data = self.audio_stream.read(512)
    print(data)
    return data
    
if __name__ == "__main__":
  # init argparse
  parser = argparse.ArgumentParser(description="Bi-directional audio pipe server")
  parser.add_argument('-i', '--input', action='store_true', default=False, help='capture windows audio')
  parser.add_argument('-s', '--audio-device', metavar='id', help='select audio device')
  args = parser.parse_args()

  # init pyaudio
  print("** audio output init")
  pa = pyaudio.PyAudio()

  # init pipe server
  print("** pipe server init")
  pipe_server = PipeServer("127.0.0.1", 24325, AudioDevice())

  # select audio input/output mode
  if args.input:
    # audio capture mode
    pipe_server.input_mode = True
    if not args.audio_device:
      pipe_server.audio_device.audio_device_select(pa)
  else:
    # output mode
    pipe_server.audio_device.info = pa.get_default_output_device_info()
  
  # manual device selection
  if args.audio_device:
    pipe_server.audio_device.info = pa.get_device_info_by_index(int(args.audio_device))

  samplerate = int(pipe_server.audio_device.info['defaultSampleRate'])
  print(f"\n*** selected mode: {'AUDIO INPUT -> VST' if args.input else 'AUDIO OUTPUT <- VST'}")
  print(f"*** selected device: {pipe_server.audio_device.info['name']}")
  print(f"*** sample rate: {samplerate} kHz")

  # run socket thread
  pipe_server.running = True
  sock_thread = threading.Thread(target=pipe_server.run())
  sock_thread.start()
  while True:
    time.sleep(0.25)
