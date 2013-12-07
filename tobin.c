#include <getopt.h>
#include <stdio.h>
#include <errno.h>

#include "extmods.h"

#include "tobin.h"


char tobin_name[] = "tobin";
int tobin_name_len = sizeof(tobin_name);

void tobin_usage(void)
{
	printf("Usage: tobin VALUE\n"
		   "Print info about passed value.\n"
		   "\n"
		   "Options:\n"
		   "  -h|--help   Print this help\n");
}


/*
 * Command line arguments parse result
 */
char opt_exit = 0;

/*
 * Parse command line arguments.
 * Returns:
 *     0 --- all ok;
 *     -EINVAL --- parse error
 * in this case return code will be written to retcode).
 */
int tobin_parse_arguments(int argc, char *argv[])
{
	int c;
	static struct option long_options[] = {
		{"help", no_argument, 0, 'h' },
		{0,      0,           0, 0   }
	};

	while (1) {
        int option_index = 0;

		c = getopt_long(argc, argv, "h",
						long_options, &option_index);
		if (c == -1)
			break;
		
		switch (c) {
		case '?':
			return -EINVAL;
		case 'h':
			tobin_usage();
			opt_exit = 1;
			return 0;
		}
	}
	
	return 0;  
}


int do_tobin(int argc, char *argv[])
{
	int ret;
	
	printf("TOBIN SPEAKS\n");

	ret = tobin_parse_arguments(argc, argv);
	if (ret < 0 || opt_exit)
		return ret;

	printf("DOIN' TOBIN STUFF\n");

	return ret;
}

EXTMOD_DECL(tobin, do_tobin, 0)
EXTMOD_DECL(tobineehy, do_tobin, 0)
EXTMOD_DECL(tobin_2, do_tobin, 0)
