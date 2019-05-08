import time
import hexdump
import pyaudio
import win32pipe, win32file, pywintypes

def client():
  print("** pipe client init")
  while 1:
    try:
      hndl = win32file.CreateFile(
        r'\\.\pipe\vstpipe1',
        win32file.GENERIC_READ,
        0,
        None,
        win32file.OPEN_EXISTING,
        0,
        None)

      print("** pipe connected")
      while True:
        adata = win32file.ReadFile(hndl, 4*1024)[1]
        stream.write(adata)
        #hexdump.hexdump(resp[1])
        
    except pywintypes.error as e:
      if e.args[0] == 2:
        print("* no pipe found")
        time.sleep(1)
      elif e.args[0] == 109:
        print("** pipe closed")

if __name__ == '__main__':
  p = pyaudio.PyAudio()
  stream = p.open(format=pyaudio.paFloat32,
                  channels=1,
                  rate=44100,
                  output=True)
             
  print("** audio output init")
  client()
