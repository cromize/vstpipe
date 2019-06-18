# vstpipe

vstpipe is simple VST plugin for piping audio out of DAW. It uses socket pipe for audio stream.  

This could be useful for piping DAW audio out to headphones connected to internal soundcard, when your external soundcard doesn't have separate channel for headphones. Or for piping audio into obs, when low-latency ASIO is used as main driver.

Audio is sent as 32-bit interleaved stereo. You can play pipe output using included server.py. Buffer size is set automatically. 

You need to include VST 2.4 SDK in Visual Studio dependencies to compile from source.


