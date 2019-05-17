import time
import win32pipe, win32file, pywintypes

if __name__ == "__main__":
  reading = False
  while 1:
    try:
      hndl = win32file.CreateFile(
        r'\\.\pipe\vstpipedebug',
        win32file.GENERIC_READ | win32file.GENERIC_WRITE,
        0,
        None,
        win32file.OPEN_EXISTING,
        0,
        None)
      reading = True
    except pywintypes.error as e:
      time.sleep(1)
      print("* no pipe found")

    while reading:
      try:
        adata = win32file.ReadFile(hndl, 8192)[1]
        print(adata.decode('ascii'))
      except pywintypes.error as e:
        win32file.CloseHandle(hndl)
        print("** pipe closed")
        reading = False
