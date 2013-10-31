#include <stdio.h>
#include <getopt.h>


/*
 * Command line options flags
 */
char opt_exit = 0;


void usage(void)
{
	printf("Usage: tobin VALUE\n"
		   "Print info about passed value.\n"
		   "\n"
		   "Options:\n"
		   "  -h|--help   Print this help\n");
}


int parse_arguments(int argc, char *argv[])
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
			return 1;
		case 'h':
			usage();
			opt_exit = 1;
			return 0;
		}
	}
	
	return 0;  
}


int main(int argc, char *argv[])
{
	if (parse_arguments(argc, argv))
		return 1;

	if (opt_exit)
		return 0;
	
	return 0;
}
