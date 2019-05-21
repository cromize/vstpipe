import time
import pyaudio
import win32pipe, win32file, pywintypes

class Pipe():
  def __init__(self, hndl=0):
    self.server_running = False
    self.client_running = False
    self.hndl = hndl

  def pipe_init(self):
    self.hndl = win32file.CreateFile(
      r'\\.\pipe\vstpipe1',
      win32file.GENERIC_READ | win32file.GENERIC_WRITE,
      0,
      None,
      win32file.OPEN_EXISTING,
      0,
      None)
    self.server_running = True

  def pipe_read(self, in_data, frame_count, time_info, status):
    try:
      #adata = win32file.ReadFile(self.hndl, 2*4*1024)[1]
      adata = win32file.ReadFile(self.hndl, 2*4*2048)[1]
      #print(adata)
      return (adata, pyaudio.paContinue)
    except pywintypes.error as e:
      if e.args[0] == 109 or e.args[0] == 233:
        print("** pipe closed")
        self.running = False
        self.server_running = False
        win32file.CloseHandle(self.hndl)
      return (None, pyaudio.paAbort)

def client_start(pipe):
  try:
    pipe.pipe_init()
    stream = pa.open(format=pyaudio.paFloat32,
                    channels=2,
                    rate=44100,
                    frames_per_buffer=2048,
                    output=True,
                    stream_callback=pipe.pipe_read)
    print("** pipe connected")
    stream.start_stream()
    pipe.running = True
    while pipe.running:
      time.sleep(0.2)
  except pywintypes.error as e:
    if not pipe.server_running:
      time.sleep(1)
      print("* cannot bind to pipe (restart the playback in host?)")
    elif e.args[0] == 2 or e.args[0] == 231:
      time.sleep(1)
      print("* no pipe found")

if __name__ == '__main__':
  pa = pyaudio.PyAudio()
  print("** audio output init")
  pipe = Pipe()
  print("** pipe client init")
  while 1:
    client_start(pipe)
