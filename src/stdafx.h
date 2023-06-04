// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifdef _WIN32


#pragma once

#define  _CRT_SECURE_NO_WARNINGS 1
#define FD_SETSIZE      256
//#define _USE_32BIT_TIME_T

// Windows Header Files:
#include <afxsock.h>		// MFC socket extensions
#include "targetver.h"

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <time.h>
#include <io.h>
#include <malloc.h>
#include <memory.h>



// TODO: reference additional headers your program requires here
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>    
#endif // _WIN32