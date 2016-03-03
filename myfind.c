/**
 * @file myfind.c
 * 
 * Beispiel 1
 * 
 * @author Florian Froestl <florian.froestl@technikum-wien.at>
 * @author Markus Diewald <ic15b068@technikum-wien.at>
 * @author
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

/* Constants */
/* Error messages */
const char * err_msg_unknown_pred = "unknown predicate: ";
const char * err_msg_missing_arg = "missing argument to ";
/* Commands */
const char * command_ls = "-ls";
const char * command_print = "-print";
const char * command_user = "-user";
const char * command_nouser = "-nouser";
const char * command_name = "-name";
const char * command_type = "-type";
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
int do_ls(char * path, char * param);

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param path [description]
 * @param name [description]
 * 
 * @return [description]
 */
int do_name(char * path, char * name);

/**
 * @brief checks a file if the owner of it equals the entered user
 * @details function checks the file if the entered user equals the owner of the file
 * 
 * @param path path including filename which needs to be checked
 * @param user username or UID
 * 
 * @return [description]
 */
int do_user(char * path, char * param)
{
	struct stat buf;
	long int uid;
	long int item_uid;
	struct passwd *get_uid;
	char *pEnd;
	
	if(stat(path, &buf) == -1)
	{
		perror("stat");
		return EXIT_FAILURE;
	}
	item_uid = buf.st_uid;
	get_uid = getpwnam(param);
	if(get_uid == NULL) /* no existing user with entered username */
		uid = strtol(param,&pEnd,10); /* parameter is an UID */
	else
		uid = get_uid->pw_uid;	/* parameter is a username */


	if(item_uid == uid)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;	
}

/**
 * @brief checks a file if it has a valid owner
 * @details function checks the file if it has a valid owner
 * 
 * @param path path including filename which needs to be checked
 * @param param always NULL
 * 
 * @return returns EXIT_SUCCESS, if the file has no valid owner, EXIT_FAILURE otherwise
 */
int do_nouser(char * path, char * param /* = NULL */)
{
	struct passwd *get_uid;
	struct stat buf;
	int item_uid;
	int uid;

	if(stat(path, &buf) == -1)
	{
		perror("stat");
		return EXIT_FAILURE;
	}
	item_uid = buf.st_uid;

	get_uid = getpwuid(item_uid);
	if(get_uid == NULL)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
	
}

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param path [description]
 * @param pattern [description]
 * 
 * @return [description]
 */
int do_path(char * path, char * pattern);

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
 		if(parseParams(params) == EXIT_FAILURE) 
 		{ 
 			return EXIT_FAILURE; 
 		} 
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
 			//do_file(path, params); 
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
		/*
		else if(strcmp((*params), command_ls) == 0)
		{
			m_Parameters[index].func = &do_ls;
			m_Parameters[index].param = NULL;
			containsPrint = true;	
		}
		*/
		else if(strcmp((*params), command_user) == 0)
		{
			m_Parameters[index].func = &do_user;
			m_Parameters[index].param = *(params+1);
			params++;
		}
		else if(strcmp((*params), command_nouser) == 0)
		{
			m_Parameters[index].func = &do_nouser;
			m_Parameters[index].param = NULL;
		}
		else
		{
			// print error message
			printf("%s%s\n", err_msg_unknown_pred, (*params));
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
