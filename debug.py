import time
import win32pipe, win32file, pywintypes

# vstpipe0 is debug pipe

if __name__ == "__main__":
  reading = False
  while 1:
    try:
      hndl = win32file.CreateFile(
        r'\\.\pipe\vstpipe0',
        win32file.GENERIC_READ | win32file.GENERIC_WRITE,
        0,
        None,
        win32file.OPEN_EXISTING,
        0,
        None)
      reading = True
    except pywintypes.error as e:
      time.sleep(1)
      print("* no debug pipe found")
      continue

    print("** debug pipe connected")
    while reading:
      try:
        adata = win32file.ReadFile(hndl, 8192)[1]
        print(adata.decode('ascii'), end='')
      except pywintypes.error as e:
        win32file.CloseHandle(hndl)
        print("** debug pipe closed")
        reading = False
