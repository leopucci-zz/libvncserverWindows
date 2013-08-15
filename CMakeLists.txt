cmake_minimum_required(VERSION 2.6)

project(LibVNCServer)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckTypeSize)
include(TestBigEndian)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
include(CheckCSourceRuns)

set(PACKAGE_NAME           "LibVNCServer")
set(FULL_PACKAGE_NAME      "LibVNCServer")
set(PACKAGE_VERSION        "0.9.10")
set(VERSION_MAJOR          "0")
set(VERSION_MINOR          "9")
set(VERSION_PATCHLEVEL     "10")
set(PROJECT_BUGREPORT_PATH "http://sourceforge.net/projects/libvncserver")

if(MSVC)
set(CMAKE_C_FLAGS "-DWIN32 -Wall")
# Microsoft deprecate certain POSIX functions that we use.
# for now, turn off these warnings.
ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
else()
set(CMAKE_C_FLAGS "-O2 -W -Wall -g")
endif()

# If the user hasn't told use specifically what they want, build only
# a static library.
IF(NOT DEFINED BUILD_SHARED AND NOT DEFINED BUILD_STATIC)
    SET(BUILD_STATIC TRUE)
    SET(BUILD_SHARED FALSE)
ENDIF(NOT DEFINED BUILD_SHARED AND NOT DEFINED BUILD_STATIC)

set(LIBVNCSERVER_DIR ${CMAKE_SOURCE_DIR}/libvncserver)
set(COMMON_DIR ${CMAKE_SOURCE_DIR}/common)
set(LIBVNCCLIENT_DIR ${CMAKE_SOURCE_DIR}/libvncclient)
add_subdirectory(examples)
add_subdirectory(client_examples)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/libvncserver ${CMAKE_SOURCE_DIR}/common)

find_package(ZLIB)
find_package(JPEG)
find_package(PNG)
find_package(SDL)
find_package(GnuTLS)
find_package(Threads)
find_package(X11)
find_package(OpenSSL)
find_library(LIBGCRYPT_LIBRARIES gcrypt)

# Check whether the version of libjpeg we found was libjpeg-turbo and print a
# warning if not.
set(CMAKE_REQUIRED_LIBRARIES ${JPEG_LIBRARIES})
set(CMAKE_REQUIRED_FLAGS -I${JPEG_INCLUDE_DIR})

set(JPEG_TEST_SOURCE "\n
  #include <stdio.h>\n
  #include <jpeglib.h>\n
  int main(void) {\n
    struct jpeg_compress_struct cinfo;\n
    struct jpeg_error_mgr jerr;\n
    cinfo.err=jpeg_std_error(&jerr);\n
    jpeg_create_compress(&cinfo);\n
    cinfo.input_components = 3;\n
    jpeg_set_defaults(&cinfo);\n
    cinfo.in_color_space = JCS_EXT_RGB;\n
    jpeg_default_colorspace(&cinfo);\n
    return 0;\n
  }")

if(CMAKE_CROSSCOMPILING)
  check_c_source_compiles("${JPEG_TEST_SOURCE}" FOUND_LIBJPEG_TURBO)
else()
  check_c_source_runs("${JPEG_TEST_SOURCE}" FOUND_LIBJPEG_TURBO)
endif()

set(CMAKE_REQUIRED_LIBRARIES)
set(CMAKE_REQUIRED_FLAGS)
set(CMAKE_REQUIRED_DEFINITIONS)

if(NOT FOUND_LIBJPEG_TURBO)
  message(WARNING "*** The libjpeg library you are building against is not libjpeg-turbo.  Performance will be reduced.  You can obtain libjpeg-turbo from:  https://sourceforge.net/projects/libjpeg-turbo/files/ ***")
endif()

# On systems such as GNU/Linux with glibc, __b64_ntop is defined in a
# separate library, libresolv. On some others, such as FreeBSD, it is
# part of libc itself. We first check if __b64_ntop is found without
# additional libraries, and then try looking for it with libresolv if
# the first test fails.
check_function_exists(__b64_ntop HAVE_B64_IN_LIBC)
if(NOT HAVE_B64_IN_LIBC)
    set(CMAKE_REQUIRED_LIBRARIES resolv)
    check_function_exists(__b64_ntop HAVE_B64_IN_LIBRESOLV)
    set(CMAKE_REQUIRED_LIBRARIES)

    if(HAVE_B64_IN_LIBRESOLV)
        set(RESOLV_LIB "resolv")
    endif(HAVE_B64_IN_LIBRESOLV)
endif(NOT HAVE_B64_IN_LIBC)

if(Threads_FOUND)
if(MSVC)
  # Currently do not support filetransfer on native MSVC
  option(TIGHTVNC_FILETRANSFER "Enable filetransfer" OFF)
  # Enable IPv6 by default
  option(LIBVNCSERVER_IPv6 "Enable IPV6" ON)
  set(LIBVNCSERVER_IPv6 1)
else()
  option(TIGHTVNC_FILETRANSFER "Enable filetransfer" ON)
endif()
endif(Threads_FOUND)
if(ZLIB_FOUND)
  set(LIBVNCSERVER_HAVE_LIBZ 1)
endif(ZLIB_FOUND)
if(JPEG_FOUND)
  set(LIBVNCSERVER_HAVE_LIBJPEG 1)
endif(JPEG_FOUND)
if(PNG_FOUND)
  set(LIBVNCSERVER_HAVE_LIBPNG 1)
endif(PNG_FOUND)
option(LIBVNCSERVER_ALLOW24BPP "Allow 24 bpp" ON)

if(GNUTLS_FOUND)
  set(LIBVNCSERVER_WITH_CLIENT_TLS 1)
  option(LIBVNCSERVER_WITH_WEBSOCKETS "Build with websockets support (gnutls)" ON)
  set(WEBSOCKET_LIBRARIES ${RESOLV_LIB} ${GNUTLS_LIBRARIES})
  set(WSSRCS ${LIBVNCSERVER_DIR}/rfbssl_gnutls ${LIBVNCSERVER_DIR}/rfbcrypto_gnutls)
elseif(OPENSSL_FOUND)
  option(LIBVNCSERVER_WITH_WEBSOCKETS "Build with websockets support (openssl)" ON)
  set(WEBSOCKET_LIBRARIES ${RESOLV_LIB} ${OPENSSL_LIBRARIES})
  set(WSSRCS ${LIBVNCSERVER_DIR}/rfbssl_openssl ${LIBVNCSERVER_DIR}/rfbcrypto_openssl)
else()
  option(LIBVNCSERVER_WITH_WEBSOCKETS "Build with websockets support (no ssl)" ON)
  set(WEBSOCKET_LIBRARIES ${RESOLV_LIB})
  set(WSSRCS ${LIBVNCSERVER_DIR}/rfbssl_none.c ${LIBVNCSERVER_DIR}/rfbcrypto_included.c ${COMMON_DIR}/md5.c ${COMMON_DIR}/sha1.c)
endif()

if(LIBGCRYPT_LIBRARIES)
  message(STATUS "Found libgcrypt: ${LIBGCRYPT_LIBRARIES}")
  set(LIBVNCSERVER_WITH_CLIENT_GCRYPT 1)
  set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} ${LIBGCRYPT_LIBRARIES})
endif(LIBGCRYPT_LIBRARIES)


check_include_file("endian.h"      LIBVNCSERVER_HAVE_ENDIAN_H)
check_include_file("fcntl.h"       LIBVNCSERVER_HAVE_FCNTL_H)
check_include_file("netinet/in.h"  LIBVNCSERVER_HAVE_NETINET_IN_H)
check_include_file("sys/endian.h"  LIBVNCSERVER_HAVE_SYS_ENDIAN_H)
check_include_file("sys/socket.h"  LIBVNCSERVER_HAVE_SYS_SOCKET_H)
check_include_file("sys/stat.h"    LIBVNCSERVER_HAVE_SYS_STAT_H)
check_include_file("sys/time.h"    LIBVNCSERVER_HAVE_SYS_TIME_H)
check_include_file("sys/types.h"   LIBVNCSERVER_HAVE_SYS_TYPES_H)
check_include_file("sys/wait.h"    LIBVNCSERVER_HAVE_SYS_WAIT_H)
check_include_file("unistd.h"      LIBVNCSERVER_HAVE_UNISTD_H)
if(MSVC)
check_include_file("strings.h"     LIBVNCSERVER_HAVE_STRINGS_H)
check_include_file("Ws2tcpip.h"    LIBVNCSERVER_HAVE_WS2TCPIP_H)
endif()

# headers needed for check_type_size()
check_include_file("arpa/inet.h"   HAVE_ARPA_INET_H)
check_include_file("stdint.h"      HAVE_STDINT_H)
check_include_file("stddef.h"      HAVE_STDDEF_H)
check_include_file("sys/types.h"   HAVE_SYS_TYPES_H)

check_function_exists(gettimeofday    LIBVNCSERVER_HAVE_GETTIMEOFDAY)

if(CMAKE_USE_PTHREADS_INIT)
  set(LIBVNCSERVER_HAVE_LIBPTHREAD 1)
endif(CMAKE_USE_PTHREADS_INIT)
if(LIBVNCSERVER_HAVE_SYS_SOCKET_H)
  # socklen_t
  list(APPEND CMAKE_EXTRA_INCLUDE_FILES "sys/socket.h")
endif(LIBVNCSERVER_HAVE_SYS_SOCKET_H)
if(HAVE_ARPA_INET_H)
  # in_addr_t
  list(APPEND CMAKE_EXTRA_INCLUDE_FILES "arpa/inet.h")
endif(HAVE_ARPA_INET_H)

check_type_size(pid_t     LIBVNCSERVER_PID_T)
check_type_size(size_t    LIBVNCSERVER_SIZE_T)
check_type_size(socklen_t LIBVNCSERVER_SOCKLEN_T)
check_type_size(in_addr_t LIBVNCSERVER_IN_ADDR_T)
if(NOT HAVE_LIBVNCSERVER_IN_ADDR_T)
  set(LIBVNCSERVER_NEED_INADDR_T 1)
endif(NOT HAVE_LIBVNCSERVER_IN_ADDR_T)

if(MSVC)
# TEST_BIG_ENDIAN(LIBVNCSERVER_WORDS_BIGENDIAN)
else()
TEST_BIG_ENDIAN(LIBVNCSERVER_WORDS_BIGENDIAN)
endif()

# TODO:
# LIBVNCSERVER_ENOENT_WORKAROUND
# inline

configure_file(${CMAKE_SOURCE_DIR}/rfb/rfbconfig.h.cmake ${CMAKE_BINARY_DIR}/rfb/rfbconfig.h)
configure_file(${CMAKE_SOURCE_DIR}/rfb/rfbint.h.cmake ${CMAKE_BINARY_DIR}/rfb/rfbint.h)

set(LIBVNCSERVER_SOURCES
    ${LIBVNCSERVER_DIR}/main.c
    ${LIBVNCSERVER_DIR}/rfbserver.c
    ${LIBVNCSERVER_DIR}/rfbregion.c
    ${LIBVNCSERVER_DIR}/auth.c
    ${LIBVNCSERVER_DIR}/sockets.c
    ${LIBVNCSERVER_DIR}/stats.c
    ${LIBVNCSERVER_DIR}/corre.c
    ${LIBVNCSERVER_DIR}/hextile.c
    ${LIBVNCSERVER_DIR}/rre.c
    ${LIBVNCSERVER_DIR}/translate.c
    ${LIBVNCSERVER_DIR}/cutpaste.c
    ${LIBVNCSERVER_DIR}/httpd.c
    ${LIBVNCSERVER_DIR}/cursor.c
    ${LIBVNCSERVER_DIR}/font.c
    ${LIBVNCSERVER_DIR}/draw.c
    ${LIBVNCSERVER_DIR}/selbox.c
    ${COMMON_DIR}/d3des.c
    ${COMMON_DIR}/vncauth.c
    ${LIBVNCSERVER_DIR}/cargs.c
    ${COMMON_DIR}/minilzo.c
    ${LIBVNCSERVER_DIR}/ultra.c
    ${LIBVNCSERVER_DIR}/scale.c
)

set(LIBVNCCLIENT_SOURCES
    ${LIBVNCCLIENT_DIR}/cursor.c
    ${LIBVNCCLIENT_DIR}/listen.c
    ${LIBVNCCLIENT_DIR}/rfbproto.c
    ${LIBVNCCLIENT_DIR}/sockets.c
    ${LIBVNCCLIENT_DIR}/vncviewer.c
    ${COMMON_DIR}/minilzo.c
)

if(MSVC)
  set(LIBVNCCLIENT_SOURCES
	${COMMON_DIR}/winapi.c
    ${LIBVNCCLIENT_SOURCES}
	)
  set(LIBVNCSERVER_SOURCES
	${COMMON_DIR}/winapi.c
	${COMMON_DIR}/base64.c
    ${LIBVNCSERVER_SOURCES}
	)
endif()

if(GNUTLS_FOUND)
  set(LIBVNCCLIENT_SOURCES
    ${LIBVNCCLIENT_SOURCES}
    ${LIBVNCCLIENT_DIR}/tls_gnutls.c
  )
elseif(OPENSSL_FOUND)
  set(LIBVNCCLIENT_SOURCES
    ${LIBVNCCLIENT_SOURCES}
    ${LIBVNCCLIENT_DIR}/tls_openssl.c
  )
else()
  set(LIBVNCCLIENT_SOURCES
    ${LIBVNCCLIENT_SOURCES}
    ${LIBVNCCLIENT_DIR}/tls_none.c
  )
endif()

if(ZLIB_FOUND)
  add_definitions(-DLIBVNCSERVER_HAVE_LIBZ)
  include_directories(${ZLIB_INCLUDE_DIR})
  set(LIBVNCSERVER_SOURCES
    ${LIBVNCSERVER_SOURCES}
    ${LIBVNCSERVER_DIR}/zlib.c
    ${LIBVNCSERVER_DIR}/zrle.c
    ${LIBVNCSERVER_DIR}/zrleoutstream.c
    ${LIBVNCSERVER_DIR}/zrlepalettehelper.c
  )
  if(MSVC)
	# according to Zlib's ReadMe, have to Define ZLIB_WINAPI for windows.
    # zlib.h will include windows.h
    ADD_DEFINITIONS(-DZLIB_WINAPI -DWIN32_LEAN_AND_MEAN)
  endif(MSVC)
endif(ZLIB_FOUND)

if(JPEG_FOUND)
  add_definitions(-DLIBVNCSERVER_HAVE_LIBJPEG)
  include_directories(${JPEG_INCLUDE_DIR})
  set(TIGHT_C ${LIBVNCSERVER_DIR}/tight.c ${COMMON_DIR}/turbojpeg.c)
endif(JPEG_FOUND)

if(PNG_FOUND)
  add_definitions(-DLIBVNCSERVER_HAVE_LIBPNG)
  include_directories(${PNG_PNG_INCLUDE_DIR})
  # set(TIGHT_C ${LIBVNCSERVER_DIR}/tight.c ${COMMON_DIR}/turbojpeg.c)
endif(PNG_FOUND)

set(LIBVNCSERVER_SOURCES
    ${LIBVNCSERVER_SOURCES}
    ${TIGHT_C}
)

if(TIGHTVNC_FILETRANSFER)
  set(LIBVNCSERVER_SOURCES
    ${LIBVNCSERVER_SOURCES}
    ${LIBVNCSERVER_DIR}/tightvnc-filetransfer/rfbtightserver.c
    ${LIBVNCSERVER_DIR}/tightvnc-filetransfer/handlefiletransferrequest.c
    ${LIBVNCSERVER_DIR}/tightvnc-filetransfer/filetransfermsg.c
    ${LIBVNCSERVER_DIR}/tightvnc-filetransfer/filelistinfo.c
  )
endif(TIGHTVNC_FILETRANSFER)

if(LIBVNCSERVER_WITH_WEBSOCKETS)
  add_definitions(-DLIBVNCSERVER_WITH_WEBSOCKETS)
  set(LIBVNCSERVER_SOURCES
    ${LIBVNCSERVER_SOURCES}
    ${LIBVNCSERVER_DIR}/websockets.c
    ${WSSRCS}
  )
endif(LIBVNCSERVER_WITH_WEBSOCKETS)

if(WIN32)
  set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} ws2_32)
endif(WIN32)

if(BUILD_SHARED)
add_library(vncclient SHARED ${LIBVNCCLIENT_SOURCES})
set_target_properties(vncclient PROPERTIES DEFINE_SYMBOL DLLDEFINE)
target_link_libraries(vncclient
                      ${ADDITIONAL_LIBS}
                      ${ZLIB_LIBRARIES}
                      ${JPEG_LIBRARIES}
)

add_library(vncserver SHARED ${LIBVNCSERVER_SOURCES})
set_target_properties(vncserver PROPERTIES DEFINE_SYMBOL DLLDEFINE)
target_link_libraries(vncserver
                      ${ADDITIONAL_LIBS}
                      ${ZLIB_LIBRARIES}
                      ${JPEG_LIBRARIES}
				      ${PNG_LIBRARIES}
				      ${WEBSOCKET_LIBRARIES}
)
SET_TARGET_PROPERTIES(vncclient vncserver
PROPERTIES SOVERSION "0.0.0"
)
else(BUILD_SHARED)
if(MSVC)
  # Use the static C library for all build types
  foreach(var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
    CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
    if(${var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${var} "${${var}}")
    endif()
  endforeach()

  add_definitions(-W3 -wd4996)
endif()
add_library(vncclient-static STATIC ${LIBVNCCLIENT_SOURCES})
target_link_libraries(vncclient-static
                      ${ADDITIONAL_LIBS}
                      ${ZLIB_LIBRARIES}
                      ${JPEG_LIBRARIES}
)

add_library(vncserver-static STATIC ${LIBVNCSERVER_SOURCES})
target_link_libraries(vncserver-static
                      ${ADDITIONAL_LIBS}
                      ${ZLIB_LIBRARIES}
                      ${JPEG_LIBRARIES}
                      ${PNG_LIBRARIES}
                      ${WEBSOCKET_LIBRARIES}
)
SET_TARGET_PROPERTIES(vncclient-static vncserver-static
PROPERTIES SOVERSION "0.0.0"
)
endif(BUILD_SHARED)


if(BUILD_SHARED)
install_targets(/lib vncserver)
install_targets(/lib vncclient)
else(BUILD_SHARED)
install_targets(/lib vncserver-static)
install_targets(/lib vncclient-static)
endif(BUILD_SHARED)

install_files(/include/rfb FILES
    rfb/keysym.h
    rfb/rfb.h
    rfb/rfbclient.h
    rfb/rfbconfig.h
    rfb/rfbint.h
    rfb/rfbproto.h
    rfb/rfbregion.h
)
