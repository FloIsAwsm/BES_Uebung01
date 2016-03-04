/**
 * @file main.c
 * 
 * Beispiel 1
 * 
 * @author Florian Froestl <florian.froestl@technikum-wien.at>
<<<<<<< HEAD
 * @author Markus Diewald <ic15b068@technikum-wien.at>
 * @author
=======
 * @author David Boisits <david.boisits@technikum-wien.at>
 * @author Markus Diewald <markus.diewald@technikum-wien.at>
>>>>>>> 677b5f897b4c7979cb110f02cbc2c6568c29a190
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
<<<<<<< HEAD
{	
	
=======
{
>>>>>>> 677b5f897b4c7979cb110f02cbc2c6568c29a190
	/* first write the application name to a global variable */
	app_name = argv[0];
	//char ** temp = argv;
	/* prevent warnings regarding unused params */
	argc = argc;

<<<<<<< HEAD
=======
	/* now print the application name and the parameters 	
	while(*temp != NULL)
	{
		printf("%s \n", *(temp));
		temp++;
	}
	printf("\n");
	*/
>>>>>>> 677b5f897b4c7979cb110f02cbc2c6568c29a190
	if(argv[1] != NULL && IsValidPath(argv[1]))
	{
		return do_dir(argv[1], (argv+2));
	}
	else
	{
<<<<<<< HEAD
		printf("%s: '%s': No such file or directory\n", argv[0], argv[1]);
	} 

	return 0;
=======
		return do_dir(".", (argv+1));
	}
<<<<<<< HEAD
>>>>>>> master
=======
	return EXIT_SUCCESS;
>>>>>>> 677b5f897b4c7979cb110f02cbc2c6568c29a190
}
