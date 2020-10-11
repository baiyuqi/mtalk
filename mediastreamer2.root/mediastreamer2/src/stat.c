/***************************************************************
  stat.c

  author : uema2
  date   : Nov 30, 2002

  You can freely use, copy, modify, and redistribute
  the whole contents.
***************************************************************/

#include <windows.h>
#include "stat.h"
#include <time.h>
#include "wince.h" /* for wce_mbtowc */

const __int64 _onesec_in100ns = (__int64)10000000;
static FILETIME wce_getFILETIMEFromYear(WORD year)
{
	SYSTEMTIME s={0};
	FILETIME f;

	s.wYear      = year;
	s.wMonth     = 1;
	s.wDayOfWeek = 1;
	s.wDay       = 1;

	SystemTimeToFileTime( &s, &f );
	return f;
}

/* __int64 <--> FILETIME */
static __int64 wce_FILETIME2int64(FILETIME f)
{
	__int64 t;

	t = f.dwHighDateTime;
	t <<= 32;
	t |= f.dwLowDateTime;
	return t;
}

time_t wce_FILETIME2time_t(const FILETIME* f)
{
	FILETIME f1601, f1970;
	__int64 t, offset;

	f1601 = wce_getFILETIMEFromYear(1601);
	f1970 = wce_getFILETIMEFromYear(1970);

	offset = wce_FILETIME2int64(f1970) - wce_FILETIME2int64(f1601);

	t = wce_FILETIME2int64(*f);

	t -= offset;
	return (time_t)(t / _onesec_in100ns);
}


int _stat(const char *filename, struct _stat *st)
{
	DWORD dwAttribute;
	HANDLE h;
	DWORD dwSizeLow=0, dwSizeHigh=0, dwError=0;
	WIN32_FIND_DATAW fd;
	wchar_t *wfilename;

	wfilename = wce_mbtowc(filename);

	dwAttribute = GetFileAttributesW(wfilename);
	if(dwAttribute==0xFFFFFFFF)
	{
		free(wfilename);
		return -1;
	}

	st->st_mode = 0;
	if((dwAttribute & FILE_ATTRIBUTE_DIRECTORY) != 0)
		st->st_mode += S_IFDIR;
	else
		st->st_mode += S_IFREG;

	/* initialize */
	st->st_atime = 0;
    st->st_mtime = 0;
    st->st_ctime = 0;
	st->st_size  = 0;
	st->st_dev   = 0;

	h = FindFirstFileW(wfilename, &fd);
	if(h == INVALID_HANDLE_VALUE)
	{
		if(wfilename[wcslen(wfilename)-1]	== L'\\')
		{
			wfilename[wcslen(wfilename)-1] = L'\0';
			h = FindFirstFileW(wfilename, &fd);
			if(h == INVALID_HANDLE_VALUE)
			{
				free(wfilename);
				return 0;
			}
		}
		else
		{
			free(wfilename);
			return 0;
		}
	}

	/* FILETIME -> time_t */
	st->st_atime = wce_FILETIME2time_t(&fd.ftLastAccessTime);
    st->st_mtime = wce_FILETIME2time_t(&fd.ftLastWriteTime);
    st->st_ctime = wce_FILETIME2time_t(&fd.ftCreationTime);
	st->st_size  = fd.nFileSizeLow;

	FindClose( h );
	free(wfilename);
	return 0;
}

int fstat(int file, struct stat *sbuf)
{
	/* GetFileSize & GetFileTime */
	DWORD dwSize;
	FILETIME ctime, atime, mtime;

	dwSize = GetFileSize( (HANDLE)file, NULL );
	if( dwSize == 0xFFFFFFFF )
		return -1;

	sbuf->st_size = dwSize;
	sbuf->st_dev  = 0;
	sbuf->st_rdev = 0;
	sbuf->st_mode = _S_IFREG;
	sbuf->st_nlink= 1;

	GetFileTime( (HANDLE)file, &ctime, &atime, &mtime );
	sbuf->st_ctime = wce_FILETIME2time_t(&ctime);
	sbuf->st_atime = wce_FILETIME2time_t(&atime);
	sbuf->st_mtime = wce_FILETIME2time_t(&mtime);

	return 0;
}

