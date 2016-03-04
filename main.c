/**
 * @file main.c
 * 
 * Beispiel 1
 * 
 * @author Florian Froestl <florian.froestl@technikum-wien.at>
 * @author David Boisits <david.boisits@technikum-wien.at>
 * @author Markus Diewald <markus.diewald@technikum-wien.at>
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
	//char ** temp = argv;
	/* prevent warnings regarding unused params */
	argc = argc;

	/* now print the application name and the parameters 	
	while(*temp != NULL)
	{
		printf("%s \n", *(temp));
		temp++;
	}
	printf("\n");
	*/
	if(argv[1] != NULL && IsValidPath(argv[1]))
	{
		return do_dir(argv[1], (argv+2));
	}
	else
	{
		return do_dir(".", (argv+1));
	}
	return EXIT_SUCCESS;
}
