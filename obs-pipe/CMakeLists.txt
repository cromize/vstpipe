project(obs-pipe)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/../source)
include_directories(SYSTEM "D:/build/obs-studio/libobs")
link_directories("D:/build/obs-studio/build/libobs")
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../source)

set(obs-pipe_SOURCES
  ${obs-pipe_PLATFORM_SOURCES}
  obs-pipe.cpp
  socket_source.cpp
  ../source/pipe.cpp
  ../source/pipe.h)

add_library(obs-pipe MODULE
  ${obs-pipe_SOURCES})

target_link_libraries(obs-pipe
  ${obs-pipe_PLATFORM_DEPS}
  libobs)
