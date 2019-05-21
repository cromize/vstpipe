# vstpipe

Simple VST plugin for piping audio out of DAW. It uses non-blocking named pipe for streaming audio. Useful for piping DAW audio out to headphones connected to internal soundcard, when your external soundcard doesn't have separate channel for headphones. Or for piping audio into obs, when low-latency ASIO is used as main driver.

Audio is sent as 32-bit interleaved stereo. You can play pipe output using included client.py.
Set the buffer size with `--buffer/-b` parameter. Buffer sizes >= 256 are tested to work well with no glitch.

You need to include VST 2.4 SDK in Visual Studio dependencies to compile from source.

Pipe name: "\\.\pipe\vstpipe1"
