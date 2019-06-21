# vstpipe

vstpipe is simple VST plugin for bi-directional audio piping into DAW. It uses socket pipe for audio stream and could easily work over network.

Audio is sent as 32-bit interleaved stereo. You can play and route input/output using included server.py. Buffer size is set automatically. You can find pre-build version build folder. Older version using Windows namedpipe is in branch namedpipe.


Same thing can be done using JACK, but vstpipe is simpler to setup.

## Usage
```
# put VST into DAW

# DESKTOP AUDIO <-- VST mode
./server.py 

# DESKTOP AUDIO --> VST mode
./server.py -i
```

* **DESKTOP AUDIO <-- VST** mode could be useful for piping DAW audio out to headphones connected to internal soundcard, when your external soundcard doesn't have separate channel for headphones. Or for piping audio into obs, when low-latency ASIO is used as main driver.

* **DESKTOP AUDIO --> VST** mode can be used to capture desktop audio from speakers into your DAW.

## Notes

* https://github.com/intxcc/pyaudio_portaudio/ is required to be able to capture desktop output using pyaudio. It contains small patch that allows to record desktop output.
* You need to include VST 2.4 SDK in Visual Studio dependencies to compile from source.
