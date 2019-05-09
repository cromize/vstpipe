# vstpipe

Not designed for low-latency stream! It kinda works for buffer size >= 256.

Simple VST plugin for piping audio out of DAW. It uses non-blocking named pipe for streaming audio. Useful for piping DAW audio into obs, when low-latency ASIO is used as main driver.

Plugin acts as pipe server, that periodically sends audio buffer. Audio can be played using included client.py.

You need to include VST 2.4 SDK in Visual Studio dependencies to compile from source.

Pipe name: "\\.\pipe\vstpipe1"
