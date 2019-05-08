# vstpipe

Not designed for low-latency stream! (glitches)

Simple VST plugin for piping audio out of DAW. It uses non-blocking named pipe for streaming audio. Useful for piping DAW audio into obs, when low-latency ASIO is used as main driver.

Plugin acts as pipe server, that periodically sends audio buffer. Audio can be played over included client.py.

Pipe name: "\\.\pipe\vstpipe1"
