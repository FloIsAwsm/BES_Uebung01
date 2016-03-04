/**
 * @file myfind.c
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
 * @todo implement command functions
 * @todo complete documentation
 * @todo comment code
 * @todo use const values in function declarations if possible
 */
#include <string.h>
#include "myfind.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <fnmatch.h>

/* Constants */
/* Error messages */
const char * err_msg_unknown_pred = "unknown predicate: ";
const char * err_msg_missing_arg = "missing argument to ";
/* Commands */
const char * command_ls = "-ls";
const char * command_print = "-print";
const char * command_user = "-user";
const char * command_name = "-name";
const char * command_type = "-type";
const char * command_path = "-path";
/* directory names */
const char* upperDir = "..";
const char* currentDir = ".";

/**
 * @brief [brief description]
 * @details [long description]
 */
static bool containsPrint = false;

/**
 * @brief global variable for the application name
 */
char * app_name;

/**
 * @brief checks if the parameters are correct and makes them easier to handle
 * @details checks the validity of our the commands and fills our parameter array m_Parameters
 * 
 * @param params the parameters we have to process
 * @return EXIT_SUCCESS if all commands are understood, EXIT_FAILURE otherwise 
 */
int parseParams(char ** params);

/**
 * @brief calls the parameter functions on a path
 * @details calls all parameter functions like "-name <name>"
 * in our parameter list on every directoy and file 
 * 
 * @param path the path we have to use the parameters on
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
int handleParams(char * path);

/**
 * @brief function pointer for our array of parameter functions
 * @details defines a function pointer for our parameter functions.
 * 
 * @param c path
 * @param r specific parameter if needed (e.g.: a pattern if the function is do_name)
 * 
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
typedef int (*paramFunc) (char *, char *);

/**
 * a structure to represent a command
 */
typedef struct
{
	paramFunc func;
	char * param;
} sParam;

/**
 * @brief [brief description]
 * @details [long description]
 */
static sParam m_Parameters [MAX_PARAMS];



/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param path [description]
 * @param param [description]
 * 
 * @return [description]
 */
int do_ls(char *path, char *pattern);

/*
int do_ls(char *path, char *pattern)
{
    struct stat fileStat;
    if(stat(path,&fileStat) < 0)
    {
        return EXIT_FAILURE;
    }

    printf("Information for %s\n",path);
    printf("---------------------------\n");
    printf("File Size: \t\t%d bytes\n", (int) fileStat.st_size);
    printf("Number of Links: \t%d\n",fileStat.st_nlink);
    printf("File inode: \t\t%d\n", (int) fileStat.st_ino);

    printf("File Permissions: \t");
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");

    printf("The file %s a symbolic link\n", (S_ISLNK(fileStat.st_mode)) ? "is" : "is not");

    return EXIT_SUCCESS;
}
*/

/**
 * @brief function which matches a given path with the pattern
 * @details match the pattern with the path
 * 
 * @param path directory path
 * @param pattern to search for
 * 
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int do_name(char *path, char *pattern);

int do_name(char *path, char *pattern)
{
	int flags = 0;
	char buffer[PATH_MAX+1];
	char * temp;
	
	if(pattern[0] != '*')
	{
		int len = snprintf(buffer, sizeof(buffer)-1, "%s%s", "*", pattern);
		
		buffer[len] = 0;
		temp = buffer;
	}
	else
	{
		temp = pattern;
	}
	
	if (fnmatch(temp, path, flags) == 0)    
	{
		return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}



/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param path [description]
 * @param user [description]
 * 
 * @return [description]
 */
int do_user(char * path, char * user);

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param path [description]
 * @param pattern [description]
 * 
 * @return [description]
 */
int do_nouser(char * path, char * param /* = NULL */);

/**
 * @brief path with pattern matching
 * @details funtion to check if given pattern is in path and return complete path
 * 
 * @param path to search for
 * @param pattern to matched with the given path
 * 
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int do_path(char * path, char * pattern);

int do_path(char *path, char *pattern)
{
	int flags = 0;
	char buffer[PATH_MAX+1];
	char * temp;
	
	if(pattern[0] != '*')
	{
		int len = snprintf(buffer, sizeof(buffer)-1, "%s%s", "*", pattern);
		
		buffer[len] = 0;
		temp = buffer;
	}
	else
	{
		temp = pattern;
	}
	
	if (fnmatch(temp, path, flags) == 0)    
	{
		return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}


/**
 * @brief prints the path on the console
 * @details prints the path on the console
 * 
 * @param path the path to be printed
 * @param param always NULL
 * 
 * @return always EXIT_SUCCESS
 */
int do_print(char * path, char * param);

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param path [description]
 * @param type [description]
 * 
 * @return [description]
 */
int do_type(char * path, char * type);

bool IsValidPath(char * param)
{
	if(param[0] == '-')
	{
		return false;
	}
	return true;
}

int do_dir(char * dir, char ** params)
{
	DIR * pdir;		
	struct dirent * item;
	
	static bool firstEntry = true;
	if (firstEntry)
	{
		struct stat mstat;
		if(stat(dir, &mstat) != 0)
		{
			printf("%s: '%s': %s\n", app_name, dir, strerror(errno));
			return EXIT_FAILURE;
		}
		if(parseParams(params) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
		firstEntry = false;
	}

	handleParams(dir);

	if(!(pdir = opendir(dir)))
	{
		/* cannot open dir */
		return EXIT_FAILURE;
	}

	while ((item = readdir(pdir)))
	{
		char path[PATH_MAX];
		int len = snprintf(path, sizeof(path)-1, "%s/%s", dir, item->d_name);
		path[len] = 0;
		if(item->d_type == DT_DIR)
		{
			if (!(strcmp(item->d_name, currentDir) == 0 || strcmp(item->d_name, upperDir) == 0))
			{
				//handleParams(path);
				do_dir(path, params); /* what if we return with EXIT_FAILURE */
			}
		}
		else
		{
			do_file(path, params);
		}
	}
	closedir(pdir);
	return EXIT_SUCCESS;
}

int do_file(char * file, char ** params)
{
	if(params)
	{
	}
	return handleParams(file);
}

int handleParams(char * path)
{
	int index = 0;
	while(m_Parameters[index].func != NULL)
	{
		if((m_Parameters[index].func)(path, m_Parameters[index].param) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
		index++;
	}
	if(!containsPrint)
	{
		do_print(path, NULL);
	}
	return EXIT_SUCCESS;
}

int parseParams(char ** params)
{
	int index = 0;
	m_Parameters[index].func = NULL;
	while((*params) != NULL && index < (MAX_PARAMS-1))
	{
		if(strcmp((*params), command_print) == 0)
		{
			m_Parameters[index].func = &do_print;
			m_Parameters[index].param = NULL;
			containsPrint = true;
		}
		else if(strcmp((*params), command_path) == 0)
		{
			m_Parameters[index].func = &do_path;
			m_Parameters[index].param = *(params+1);;
			params++;
		}		
		else if(strcmp((*params), command_name) == 0)
		{
			m_Parameters[index].func = &do_name;
			m_Parameters[index].param = *(params+1);
			params++;
		}
		/*
		else if(strcmp((*params), command_ls) == 0)
		{
			m_Parameters[index].func = &do_ls;
			m_Parameters[index].param = NULL;
			containsPrint = true;	
		}
		*/
		else
		{
			/* print error message */
			if((*params)[0] == '-')
			{
				printf("%s%s\n", err_msg_unknown_pred, (*params));
			}
			else
			{
				printf("paths must exceed the exp...");
			}
			return EXIT_FAILURE;
		}
		index++;
		params++;
	}
	m_Parameters[index].func = NULL;
	return EXIT_SUCCESS;
}

int do_print(char * path, char * param)
{
	if(param == NULL)
	{
	}
	printf("%s\n", path);
	return EXIT_SUCCESS;
}
