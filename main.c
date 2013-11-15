#include <stdio.h>
#include <getopt.h>

#define PARSE_ABORT 0
#define PARSE_PROCEED 1


/*
 * Command line options flags
 */


void usage(void)
{
	printf("Usage: tobin VALUE\n"
		   "Print info about passed value.\n"
		   "\n"
		   "Options:\n"
		   "  -h|--help   Print this help\n");
}

/*
 * Parse command line arguments.
 * Returns PARSE_PROCEED if all ok, PARSE_ABORT --- if app should be
 * closed (in this case return code will be written to retcode).
 */
int parse_arguments(int argc, char *argv[], int *retcode)
{
	int c;
	static struct option long_options[] = {
		{"help", no_argument, 0, 'h' },
		{0,      0,           0, 0   }
	};

	if (retcode)
		*retcode = 0;

	while (1) {
        int option_index = 0;

		c = getopt_long(argc, argv, "h",
						long_options, &option_index);
		if (c == -1)
			break;
		
		switch (c) {
		case '?':
			if (retcode)
				*retcode = 1;
			return PARSE_ABORT;
		case 'h':
			usage();
			return PARSE_PROCEED;
		}
	}
	
	return 0;  
}


int main(int argc, char *argv[])
{
	int ret;
	
	if (!parse_arguments(argc, argv, &ret))
		return ret;

	return 0;
}
