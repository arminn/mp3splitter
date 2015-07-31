#include <fcntl.h>
#include <mpg123.h>
#include <stdio.h>
#include <unistd.h>

static const char* DEFAULT_OUT="./out_m.mp3";
const double timeStart = 10*60+6, timeStop = 13*60+16;

double getEstimatedTotalTime(mpg123_handle *m)
{
	off_t a = mpg123_length(m);
	double tpf = mpg123_tpf(m);
	int spf = mpg123_spf(m);
	return (double)a/spf*tpf;
}

int main(int argc, char **argv)
{
	mpg123_handle *m;
	int i;
	if(argc < 2)
	{
		fprintf(stderr, "\nI will give you the estimated and exact sample lengths of MPEG audio files.\n");
		fprintf(stderr, "\nUsage: %s <mpeg audio file list>\n\n", argv[0]);
		return -1;
	}
	mpg123_init();
	m = mpg123_new(NULL, NULL);
	mpg123_param(m, MPG123_RESYNC_LIMIT, -1, 0); /* New in library version 0.0.1 . */
	for(i = 1; i < argc; ++i)
	{
		off_t a, b;

		mpg123_open(m, argv[i]);

		a = mpg123_length(m);
		mpg123_scan(m);
		b = mpg123_length(m);

		int outFd = open(DEFAULT_OUT,O_WRONLY |O_CREAT);
		if (outFd < 0)
			goto EXIT;

		int ret = 0;
		size_t count = 0;

		double totalTime = 0;

		off_t startOff = mpg123_timeframe(m,timeStart);
		off_t stopOff = mpg123_timeframe(m,timeStop);
		printf("Start=%ld stop=%ld\n",startOff, stopOff);
		mpg123_seek(m,startOff,SEEK_SET);
		while( (ret = mpg123_framebyframe_next(m)) == MPG123_OK || ret == MPG123_NEW_FORMAT )
		{
			unsigned long header;
			unsigned char *bodydata;
			size_t bodybytes;

			off_t currentOff = mpg123_tellframe(m)-1;


			printf("Current offset %ld \n", currentOff);
			if (currentOff < startOff)
				continue;
			if (currentOff > stopOff)
				break;

			if(mpg123_framedata(m, &header, &bodydata, &bodybytes) == MPG123_OK)
			{
				/* Need to extract the 4 header bytes from the native storage in the correct order. */
				unsigned char hbuf[4];
				int i;
				for(i=0; i<4; ++i) hbuf[i] = (unsigned char) ((header >> ((3-i)*8)) & 0xff);

				/* Now write out both header and data, fire and forget. */
				write(outFd, hbuf, 4);
				write(outFd, bodydata, bodybytes);
				fprintf(stderr, "%zu: header 0x%08x, %zu body bytes\n", ++count, header, bodybytes);
			}
		}

		printf("File %i: estimated %li vs. scanned %li\n", i, (long)a, (long)b);
		printf("Estimated time=%f\n", getEstimatedTotalTime(m));

		close(outFd);
		mpg123_close(m);
	}
EXIT:
	mpg123_delete(m);
	mpg123_exit();
	return 0;
}

