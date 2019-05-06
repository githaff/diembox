#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <extopts/extopts.h>
#include <extopts/extmods.h>

#include "common.h"
#include "config.h"


static bool opts_help;
static bool opts_version;

struct extopt timestream_opts[] = {
	EXTOPTS_HELP(&opts_help),
	EXTOPTS_VERSION(&opts_version),
	EXTOPTS_END
};

static enum time_mode {
    TM_NONE,
    TM_SEC,
    TM_MIN,
} time_mode = TM_NONE;
timer_t timerid;


static void timestream_help(void)
{
	extmod_print_desc(extmod);
}

static void timestream_version(void)
{
	printf("Timestream utility from diembox toolset %s\n", EMBOX_VERSION_FULL);
}

static void toggle_mode(int sig)
{
    (void)sig;
    int ret = 0;

    struct itimerspec its = { 0 };
    struct timespec ts;

    switch (time_mode) {
    case TM_SEC :
        time_mode = TM_MIN;
        its.it_value.tv_sec = 1;
        its.it_interval.tv_sec = 1;
        break;
    default :
    case TM_MIN :
        time_mode = TM_SEC;
        timespec_get(&ts, TIME_UTC);
        its.it_value.tv_nsec = 1;
        its.it_interval.tv_sec = 60;
        break;
    }

    ret = timer_settime(timerid, TIMER_ABSTIME, &its, NULL);
    if (ret) {
        fprintf(stderr, "Error: failed to set timer: %s\n", strerror(errno));
        return;
    }
}

static void print_time(int sig)
{
    (void)sig;

    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    char buf[128];

    switch (time_mode) {
    case TM_SEC :
        strftime(buf, sizeof(buf), "%H:%M", gmtime(&ts.tv_sec));
        break;
    case TM_MIN :
        strftime(buf, sizeof(buf), "%H:%M:%S", gmtime(&ts.tv_sec));
        break;
    default:
        strcpy(buf, "-inval-");
    }

    printf("%s\n", buf);
}

int timestream_main(int argc, char *argv[])
{
	int ret = 0;

	ret = extopts_get(&argc, argv, timestream_opts);
	if (opts_help) {
		timestream_help();
		goto end;
	}
	if (opts_version) {
		timestream_version();
		goto end;
	}

    signal(SIGUSR1, print_time);
    signal(SIGUSR2, toggle_mode);

    struct sigevent evp = {
        .sigev_notify = SIGEV_SIGNAL,
        .sigev_signo  = SIGUSR1,
    };
    ret = timer_create(CLOCK_REALTIME, &evp, &timerid);
    if (ret) {
        fprintf(stderr, "Error: failed to create timer: %s\n", strerror(errno));
        goto end;
    }

    toggle_mode(0);

    while (true)
        sleep(1);

end:
	return ret;
}

EXTMOD_DECL(timestream, timestream_main, timestream_opts,
			"Time streaming utility",
			"Usage: diembox timestream\n"
			"This utility streams the current system time aligned to seconds or minutes.\n"
			"The actual time prints are aligned to minutes or seconds depending on the\n"
			"current output mode.\n"
			"Default output mode is minutes. It can be forced to switch with USR2.\n"
			"USR1 forces to print current time immediately.\n")
