/**
 * @file main.c
 * 
 * Beispiel 1
 * 
 * @author Florian Froestl <florian.froestl@technikum-wien.at>
 * @author
 * @author
 * 
 * @date 2016/02/22
 * 
 * @version 100
 * 
 * @todo remove magic numbers
 */
#include <stdio.h>
#include "myfind.h"

int main(int argc, char** argv)
{	
	
	/* first write the application name to a global variable */
	app_name = argv[0];
	char ** temp = argv;
	/* prevent warnings regarding unused params */
	argc = argc;

	if(argv[1] != NULL && IsValidPath(argv[1]))
	{
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

	return 0;
}
