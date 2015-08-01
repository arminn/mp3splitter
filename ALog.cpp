#include <stdarg.h>
#include <string.h>
#include "ALog.h"

#define LOG_CONFIG_WITHOUT_FL

const char* Log_Mark [LOG_MAX] = {"V:", "D:", "I:", "W:", "E:"};
static const int MAX_STR_LEN = 512;
static enum Log_Severity currentLvl = LOG_VERBOSE;

void __a_log(FILE* fs, enum Log_Severity sLevel, const char* file, int fline,const char* fmt, ...)
{
	char line[MAX_STR_LEN];
	if (currentLvl > sLevel)
		return;
	int vlen;
#ifndef LOG_CONFIG_WITHOUT_FL
	vlen = snprintf(line, MAX_STR_LEN, "%s:%s:%d\t", Log_Mark[sLevel], file, fline);
#else
	vlen = snprintf(line, MAX_STR_LEN, "%s: \t", Log_Mark[sLevel]);
#endif
	va_list argptr;
	va_start ( argptr, fmt );
	//int vlen = strlen ( line ) ;
	if ( vlen > MAX_STR_LEN )  vlen = MAX_STR_LEN - 1;
	vsnprintf ( & line [ vlen ], MAX_STR_LEN - vlen - 1, fmt, argptr );
	va_end ( argptr );
	fprintf(fs,"%s", line);
	fprintf(fs, "\n");
}
