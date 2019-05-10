import time
import pyaudio
import win32pipe, win32file, pywintypes

server_running = False
running = False

def pipe_init():
  global server_running
  hndl = win32file.CreateFile(
    r'\\.\pipe\vstpipe1',
    win32file.GENERIC_READ | win32file.GENERIC_WRITE,
    0,
    None,
    win32file.OPEN_EXISTING,
    0,
    None)
  server_running = True
  return hndl

def pipe_read(in_data, frame_count, time_info, status):
  global hndl
  global running
  try:
    adata = win32file.ReadFile(hndl, 2*4*1024)[1]
    #hexdump.hexdump(adata)
    return (adata, pyaudio.paContinue)
  except pywintypes.error as e:
    if e.args[0] == 109 or e.args[0] == 233:
      print("** pipe closed")
      running = False
      server_running = False
      win32file.CloseHandle(hndl)
    return (None, pyaudio.paAbort)

def client_start(pa):
  global hndl
  global running
  global server_running
  try:
    hndl = pipe_init()
    stream = pa.open(format=pyaudio.paFloat32,
                    channels=2,
                    rate=44100,
                    output=True,
                    stream_callback=pipe_read)
    print("** pipe connected")
    stream.start_stream()
    running = True
    while running:
      time.sleep(0.1)
  except pywintypes.error as e:
    #print(e)
    if e.args[0] == 2:
      time.sleep(1)
      print("* no pipe found")
    elif e.args[0] == 231 and not server_running:
      time.sleep(1)
      print("* restart the playback in host")

if __name__ == '__main__':
  pa = pyaudio.PyAudio()
  print("** audio output init")
  print("** pipe client init")
  while 1:
    client_start(pa)
