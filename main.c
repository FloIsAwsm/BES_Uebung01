#include <unistd.h> // getcwd
#include <stdio.h>
#include "myfind.h"

int main(int argc, char** argv)
{
	char buf[MAX_PATH];
	// first write the application name to a global variable

	if(argv[1] != NULL && IsValidPath(argv[1]))
	{
		return do_dir(argv[1], argv[2]);
	}
	else
	{
		// get current working directory
		if(getcwd(buf, MAX_PATH) == NULL)
		{
			// maybe we can find another way
			return EXIT_FAILURE;
		}
		return do_dir(buf, argv[1]);
	};
}