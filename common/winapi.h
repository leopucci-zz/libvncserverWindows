#if defined(WIN32) && defined(_MSC_VER)
#ifndef _WIN_API_H_
#define _WIN_API_H_

#include <windows.h>

#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif

#define _O_RDONLY      0
#define _O_WRONLY      1
#define _O_RDWR        2
#define _O_ACCMODE     (_O_RDONLY|_O_WRONLY|_O_RDWR)
#define _O_APPEND      0x0008
#define _O_RANDOM      0x0010
#define _O_SEQUENTIAL  0x0020
#define _O_TEMPORARY   0x0040
#define _O_NOINHERIT   0x0080
#define _O_CREAT       0x0100
#define _O_TRUNC       0x0200
#define _O_EXCL        0x0400
#define _O_SHORT_LIVED 0x1000
#define _O_TEXT        0x4000
#define _O_BINARY      0x8000
#define _O_RAW         _O_BINARY


#define O_RDONLY    _O_RDONLY
#define O_WRONLY    _O_WRONLY
#define O_RDWR      _O_RDWR
#define O_ACCMODE   _O_ACCMODE
#define O_APPEND    _O_APPEND
#define O_RANDOM    _O_RANDOM
#define O_SEQENTIAL _O_SEQUENTIAL
#define O_TEMPORARY _O_TEMPORARY
#define O_NOINHERIT _O_NOINHERIT
#define O_CREAT     _O_CREAT
#define O_TRUNC     _O_TRUNC
#define O_EXCL      _O_EXCL
#define O_TEXT      _O_TEXT
#define O_BINARY    _O_BINARY
#define O_RAW       _O_BINARY



typedef unsigned int lino_t;

struct dirent {
  lino_t          d_ino;
  char           d_name[_MAX_PATH];
};

typedef struct {
  HANDLE h;
  WIN32_FIND_DATAA *fd;
  BOOL has_next;
  struct dirent entry;
} DIR;

DIR *opendir(const char *name);
struct dirent *readdir(DIR *dir);
int closedir(DIR *dir);

BOOL IsWinVistaOrLater();

#endif /*_WIN_API_H_*/
#endif

