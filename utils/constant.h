#ifndef __WLR_UTILS__H
#define __WLR_UTILS__H


#include <vector>
#include <iostream>
#include <string.h>
#include <execinfo.h>

#ifndef __linux__
#include <windows.h>
#else
#include <unistd.h>
#include <sys/syscall.h>
#endif


namespace wlr
{

#define CHAR_MAX (char)0x7F
#define CHAR_MIN (char)0xFF
#define SHORT_MAX (short)0x7FFF
#define SHORT_MIN (short)0xFFFF
#define INT_MAX 0x7FFFFFFF
#define INT_MIN 0xFFFFFFFF
#define LLONG_MAX 0x7FFFFFFFFFFFFFFFL
#define LLONG_MIN 0xFFFFFFFFFFFFFFFFL
typedef unsigned char wchar;
typedef unsigned short wshort;
typedef unsigned int wint;
typedef unsigned long wlong;
typedef unsigned long long wllong;

#ifndef __linux__
inline int processId()
{return GetCurrentProcessId();}

inline int threadId()
{return GetCurrentThreadId();}

#else
inline int processId()
{return getpid();}

//inline int threadId()
//{return gettid();}

inline int threadId()
{return syscall(SYS_gettid);}
#endif

#define W_DELETE(p) if(p){delete p; p=NULL;}
#define W_FOR(i, c, s) for(int i = 0; i c s; i++)



}

#endif
