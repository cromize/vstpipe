import time
import queue
import hexdump
import pyaudio
import threading
import win32pipe, win32file, pywintypes

q = queue.Queue()

running = True

def pipe_init():
  return win32file.CreateFile(
    r'\\.\pipe\vstpipe1',
    win32file.GENERIC_READ,
    0,
    None,
    win32file.OPEN_EXISTING,
    0,
    None)

def pipe_read(hndl):
  global running
  global q
  while running:
    try:
      adata = win32file.ReadFile(hndl, 2*4*1024)[1]
      #hexdump.hexdump(adata)
      q.put(adata)
    except pywintypes.error as e:
      if e.args[0] == 109:
        q = queue.Queue()
        win32file.CloseHandle(hndl)
        print("** pipe closed")
        running = False

def client():
  global running
  print("** pipe client init")
  while 1:
    try:
      running = True
      hndl = pipe_init()
      pipe_thread = threading.Thread(target=pipe_read, args=(hndl,))
      pipe_thread.start()
      print("** pipe connected")
      time.sleep(0.100)
      while running:
        time.sleep(0.001)
        stream.write(q.get(timeout=1))
      pipe_thread.join()
        
    except pywintypes.error as e:
      if e.args[0] == 2:
        time.sleep(1)
        print("* no pipe found")
    except queue.Empty:
      pass

if __name__ == '__main__':
  p = pyaudio.PyAudio()
  stream = p.open(format=pyaudio.paFloat32,
                  channels=1,
                  rate=44100,
                  output=True)
             
  print("** audio output init")
  client()
