#include <stdio.h>
#include "myfind.h"

int main(int argc, char** argv)
{
	// first write the application name to a global variable

	if(argv[1] != NULL && IsValidPath(argv[1]))
	{
		//printf("%s", argv[1]);
		return do_dir(argv[1], (argv+2));
	}
	else if (argv[1] != NULL)
	{
		return do_dir(".", (argv+1));
	}
	else if (argv[1] == NULL)
	{
		return do_dir(".", (argv+1));
	}
	else
	{
		printf("%s: '%s': No such file or directory\n", argv[0], argv[1]);
	}
	if(argc < 0)
	{
		return EXIT_FAILURE;
	}
}
