# tests
set(LIBVNCSERVER_TESTS
    backchannels
    camera
    colourmaptest
    example
    fontsel
    pnmshow
    pnmshow24
    regiontest
    rotate
    simple
    simple15
    storepasswd
    vncev
  )

if(Threads_FOUND)
  set(LIBVNCSERVER_TESTS
    ${LIBVNCSERVER_TESTS}
    blooptest
  )
endif(Threads_FOUND)

if(TIGHTVNC_FILETRANSFER)
  set(LIBVNCSERVER_TESTS
    ${LIBVNCSERVER_TESTS}
    filetransfer
  )
endif(TIGHTVNC_FILETRANSFER)

if(MACOS)
  set(LIBVNCSERVER_TESTS
    ${LIBVNCSERVER_TESTS}
    mac
  )
endif(MACOS)

foreach(test ${LIBVNCSERVER_TESTS})
  add_executable(${test} ${test}.c)
if(BUILD_SHARED)
  target_link_libraries(${test} vncserver ${CMAKE_THREAD_LIBS_INIT})
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

  target_link_libraries(${test} vncserver-static ${CMAKE_THREAD_LIBS_INIT})
endif(BUILD_SHARED)
endforeach(test ${LIBVNCSERVER_TESTS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/libvncserver ${CMAKE_SOURCE_DIR}/common)


