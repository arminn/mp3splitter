#ifndef ALOG_H_
#define ALOG_H_

#include <stdio.h>

enum Log_Severity {
	LOG_VERBOSE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_MAX
};

void __a_log(FILE* fs, enum Log_Severity sLevel, const char* file, int fline, const char* fmt,...);

#ifndef WITHOUT_VERBOSE
	#define LOGV(MSG,...) do {__a_log(stdout, LOG_VERBOSE, __FILE__, __LINE__, MSG, #__VA_ARGS__);}while(0)
#else
	#define LOGV(MSG,...)
#endif

#ifndef WITHOUT_DEBUG
	#define LOGD(MSG,...) do {__a_log(stdout, LOG_DEBUG, __FILE__, __LINE__, MSG, __VA_ARGS__, "");}while(0)
#else
	#define LOGD(MSG,...)
#endif

#ifndef WITHOUT_INFO
	#define LOGI(MSG,...) do {__a_log(stdout, LOG_INFO, __FILE__, __LINE__, MSG, __VA_ARGS__, "");}while(0)
#else
	#define LOGI(MSG,...)
#endif


#ifndef WITHOUT_WARNING
	#define LOGW(MSG,...) do {__a_log(stderr, LOG_WARNING, __FILE__, __LINE__, MSG, #__VA_ARGS__);}while(0)
#else
	#define LOGW(MSG,...)
#endif

#ifndef WITHOUT_ERROR
	#define LOGE(MSG,...) do {__a_log(stderr, LOG_ERROR, __FILE__, __LINE__, MSG, #__VA_ARGS__);}while(0)
#else
	#define LOGE(MSG,...)
#endif

#endif /* LOG_H_ */
