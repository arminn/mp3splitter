// TODO: For version 0.1 should be :
//  +Add possibility to split in to several files
//	+Create mechanism for at least hardcoded list of split's
//  -Create some simple input parser
//  -Add normal naming and ID3 TAG copy in to out files
//  -Add tunable input configuration
//

#include <fcntl.h>
#include <mpg123.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <string>

#include "ALog.h"

using std::string;

struct SplitNode {
	double timeStart;
	double timeStop;
	std::string compName;
};

SplitNode gNodes[] = {
		{3*60+48, 6*60+32, "Philip_George_Woodhead-Wish_You_Were_Mine"},
		{606, 13*60+16, "Sigma_ft_Paloma_Faith-Changing"}
};

double getEstimatedTotalTime(mpg123_handle *m)
{
	off_t a = mpg123_length(m);
	double tpf = mpg123_tpf(m);
	int spf = mpg123_spf(m);
	return (double)a/spf*tpf;
}


void doSplit(mpg123_handle *m, const char* outFile, const double timeStart, const double timeStop)
{
	int ret = 0;
	size_t count = 0;
	off_t startOff = mpg123_timeframe(m, timeStart);
	off_t stopOff = mpg123_timeframe(m, timeStop);

	int outFd = open(outFile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if (outFd < 0) {
		LOGE("Unable to open file[%s][%s]", outFile, strerror(errno));
		return;
	}

	LOGV("Start=%ld stop=%ld", startOff, stopOff);

	if (mpg123_seek(m, startOff, SEEK_SET) < 0) {
		LOGE("Failed to sek onto %ld", startOff);
	}
	while ( (ret = mpg123_framebyframe_next(m)) == MPG123_OK || ret == MPG123_NEW_FORMAT )
	{
		unsigned long header;
		unsigned char *bodydata;
		size_t bodybytes;

		off_t currentOff = mpg123_tellframe(m)-1;

		//printf("Current offset %ld \n", currentOff);
		if (currentOff < startOff)
			continue;
		if (currentOff > stopOff)
			break;

		if (mpg123_framedata(m, &header, &bodydata, &bodybytes) == MPG123_OK)
		{
			/* Need to extract the 4 header bytes from the native storage in the correct order. */
			unsigned char hbuf[4];
			int i;
			for (i=0; i<4; ++i) hbuf[i] = (unsigned char) ((header >> ((3-i)*8)) & 0xff);

			/* Now write out both header and data, fire and forget. */
			write(outFd, hbuf, 4);
			write(outFd, bodydata, bodybytes);
			//fprintf(stderr, "%zu: header 0x%08lx, %zu body bytes\n", ++count, header, bodybytes);
		}
	}
	close(outFd);

}

int main(int argc, char **argv)
{

	mpg123_handle *m;
	int i;
	if (argc < 3)
	{
		LOGI("Usage: %s <mpeg audio IN.mp3 OUT-MASK-NAME>\n", argv[0]);
		return -1;
	}
	mpg123_init();
	m = mpg123_new(NULL, NULL);
	mpg123_param(m, MPG123_RESYNC_LIMIT, -1, 0); /* New in library version 0.0.1 . */
	mpg123_open(m, argv[1]);
	off_t a, b;
	a = mpg123_length(m);
	mpg123_scan(m);
	b = mpg123_length(m);

	for (i = 0; i < sizeof(gNodes)/sizeof(SplitNode); ++i)
	{
		char tmp[254];
		snprintf(tmp, 254, "%s%d_%s.mp3", argv[2], i, gNodes[i].compName.c_str());

		doSplit(m, tmp, gNodes[i].timeStart, gNodes[i].timeStop);

	}

	LOGD("File %i: estimated %li vs. scanned %li", i, (long)a, (long)b);
	LOGD("Estimated time=%f\n", getEstimatedTotalTime(m));
	mpg123_close(m);
EXIT:
	mpg123_delete(m);
	mpg123_exit();
	return 0;
}

