project(obs-pipe)

include_directories(SYSTEM "D:/build/obs-studio/libobs")
link_directories("D:/build/obs-studio/build/libobs")

set(obs-pipe_SOURCES
  ${obs-pipe_PLATFORM_SOURCES}
  obs-pipe.c
  socket_source.c)

add_library(obs-pipe MODULE
  ${obs-pipe_SOURCES})

target_link_libraries(obs-pipe
  ${obs-pipe_PLATFORM_DEPS}
  libobs)
