

#ifndef _UNICODE_DEFINES_INCLUDED_
#define _UNICODE_DEFINES_INCLUDED_

#include <string>

#ifdef _UNICODE

#define fopen_t _wfopen_s
#define strtol_t wcstol
#define strtod_t wcstod
#define strcspn_t wcscspn
#define vsprintf_t  vswprintf
#define vsnprintf_t _vsnwprintf

typedef std::wstring TSTRING;

#else

#define fopen_t fopen_s
#define strtol_t strtol
#define strtod_t strtod
#define strcspn_t strcspn
#define vsprintf_t  vsprintf
#define vsnprintf_t vsnprintf

typedef std::string TSTRING;

#endif /* _UNICODE */

#endif
