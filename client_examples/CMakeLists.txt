set(LIBVNCCLIENT_TESTS
    backchannel
    ppmtest
)

if(SDL_FOUND)
  include_directories(${SDL_INCLUDE_DIR})
  set(LIBVNCCLIENT_TESTS
    ${LIBVNCCLIENT_TESTS}
    SDLvncviewer
  )
  set(SDLvncviewer_EXTRA_SOURCES scrap.c)
endif(SDL_FOUND)

if(HAVE_FFMPEG)
  set(LIBVNCCLIENT_TESTS
    ${LIBVNCCLIENT_TESTS}
    vnc2mpg
  )
endif(HAVE_FFMPEG)

foreach(test ${LIBVNCCLIENT_TESTS})
  add_executable(${test} ${test}.c ${${test}_EXTRA_SOURCES} )
if(BUILD_SHARED)
  target_link_libraries(${test} vncclient ${CMAKE_THREAD_LIBS_INIT} ${GNUTLS_LIBRARIES} ${X11_LIBRARIES} ${SDL_LIBRARY} ${FFMPEG_LIBRARIES})
else(BUILD_SHARED)
if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  /NODEFAULTLIB:LIBCMT.LIB")
  # Use the static C library for all build types
  foreach(var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
    CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
    if(${var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${var} "${${var}}")
    endif()
  endforeach()
  add_definitions(-W3 -wd4996)
endif()

  target_link_libraries(${test} vncclient-static ${CMAKE_THREAD_LIBS_INIT} ${GNUTLS_LIBRARIES} ${X11_LIBRARIES} ${SDL_LIBRARY} ${FFMPEG_LIBRARIES})
endif(BUILD_SHARED)
endforeach(test ${LIBVNCCLIENT_TESTS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/libvncclient ${CMAKE_SOURCE_DIR}/common)
