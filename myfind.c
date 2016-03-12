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
 * @todo complete documentation
 * @todo comment code
 * @todo use const values in function declarations if possible
 * @todo comment why we need includes (functions or data-types)
 */
#include <string.h> //strcmp
#include <unistd.h>
#include <stdlib.h> // calloc, free
#include <pwd.h>
#include <grp.h> // 
#include <time.h> // localtime...
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h> // readdir, opendir...
#include <stdio.h> // printf
#include <fnmatch.h> // fnmatch
#include <errno.h>
#include "myfind.h"

/* Constants */
/* defines */
#define LOG_ENABLED
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
const char * command_path = "-path";
/* directory names */
const char* upperDir = "..";
const char* currentDir = ".";

/* type names */
const char * type_dir = "d";
const char * type_block = "b";
const char * type_character = "c";
const char * type_pipe = "p";
const char * type_file = "f";
const char * type_link = "l";
const char * type_socket = "s";
//const char * type_Door = "D";

#ifdef LOG_ENABLED
int do_log(char * msg);
#endif 

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

/**
 * @brief checks a file if the owner of it equals the entered user
 * @details function checks the file if the entered user equals the owner of the file
 * 
 * @param path path including filename which needs to be checked
 * @param user username or UID
 * 
 * @return EXIT_SUCCESS if the file or directory matches the user in param, EXIT_FAILURE otherwise
 */
int do_user(char * path, char * param);

/**
 * @brief checks a file if it has a valid owner
 * @details function checks the file if it has a valid owner
 * 
 * @param path path including filename which needs to be checked
 * @param param always NULL
 * 
 * @return returns EXIT_SUCCESS, if the file has no valid owner, EXIT_FAILURE otherwise
 */
int do_nouser(char * path, char * param);

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

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param param [description]
 * @return [description]
 */
mode_t get_type(char * param);

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param path [description]
 * @return [description]
 */
char * get_Name(char * path);


bool IsValidPath(char * param)
{
	if(param == NULL)
	{
		return false;
	}

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
		printf("cannot open dir: %s\n", dir);
		return EXIT_FAILURE;
	}

	while ((item = readdir(pdir)))
	{
		//char path[PATH_MAX];
		int size = strlen(dir) + strlen(item->d_name) + 2; // terminating null + seperating /
		char * path = (char *) calloc(size, sizeof(char));
		if (path == NULL)
		{
			// @todo print error message
			printf("mem alloc failed in do_dir\n");
			return EXIT_FAILURE;
		}
		int len = snprintf(path, size, "%s/%s", dir, item->d_name);
		//int len = snprintf(path, sizeof(path)-1, "%s/%s", dir, item->d_name);
		path[len] = 0;
		if(item->d_type == DT_DIR)
		{
			if (!(strcmp(item->d_name, currentDir) == 0 || strcmp(item->d_name, upperDir) == 0))
			{
				do_dir(path, params); /* what if we return with EXIT_FAILURE */
			}
		}
		else
		{
			handleParams(path);
		}
		free(path);
	}
	closedir(pdir);
	return EXIT_SUCCESS;
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

// @todo check params+1 != NULL before writing it in the struct
// @todo write an appropriate error message if params+1 == NULL
int parseParams(char ** params)
{
#ifdef
	do_log("parseParams...");
#endif

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
		else if(strcmp((*params), command_ls) == 0)
		{
			m_Parameters[index].func = &do_ls;
			m_Parameters[index].param = NULL;
			containsPrint = true;
		}
		else if(strcmp((*params), command_path) == 0)
		{
			m_Parameters[index].func = &do_path;
			params++;
			if (*params != NULL)
			{
				m_Parameters[index].param = *(params);
			}
			else
			{
				// print error message
				return EXIT_FAILURE;
			}
		}		
		else if(strcmp((*params), command_name) == 0)
		{
			m_Parameters[index].func = &do_name;
			params++;
			if (*params != NULL)
			{
				m_Parameters[index].param = *(params);
			}
			else
			{
				// print error message
				return EXIT_FAILURE;
			}
		}
		else if(strcmp((*params), command_user) == 0)
		{
			m_Parameters[index].func = &do_user;
			params++;
			if (*params != NULL)
			{
				m_Parameters[index].param = *(params);
			}
			else
			{
				// print error message
				return EXIT_FAILURE;
			}
		}
		else if(strcmp((*params), command_nouser) == 0)
		{
			m_Parameters[index].func = &do_nouser;
			m_Parameters[index].param = NULL;
		}
		else if(strcmp((*params), command_type) == 0)
		{
			m_Parameters[index].func = &do_type;
			params++;
			if(params != NULL && *params != NULL)
			{
				if (strlen((*params)) != 1)
				{
					// @todo parameter must only be one character exception
					return EXIT_FAILURE;
				}
				if (get_type(*params) == 0)
				{
					printf("%s: unknown argument to -type: %s", app_name, *params);
					return EXIT_FAILURE;
				}
				m_Parameters[index].param = *params;
			}
			else
			{
				// @todo missing argument error
				return EXIT_FAILURE;
			}
		}
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
	param = param;
	printf("%s\n", path);
	return EXIT_SUCCESS;
}


int do_user(char * path, char * param)
{	
#ifdef
	do_log("do_user...");
#endif

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
	{
		uid = strtol(param,&pEnd,10); /* parameter is an UID */
	}
	else
	{
		uid = get_uid->pw_uid;	/* parameter is a username */
	}

	if(item_uid == uid)
	{
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}	
}

int do_nouser(char * path, char * param)
{
#ifdef
	do_log("do_nouser...");
#endif

	struct passwd *get_uid;
	struct stat buf;
	int item_uid;
	//int uid;

	// unused parameter warning
	param = param;

	if(stat(path, &buf) == -1)
	{
		perror("stat");
		return EXIT_FAILURE;
	}
	item_uid = buf.st_uid;

	get_uid = getpwuid(item_uid);
	if(get_uid == NULL)
	{
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
	
}

int do_name(char *path, char *pattern)
{
#ifdef
	do_log("do_name...");
#endif

	int flags = 0;
	
	if (fnmatch(pattern, get_Name(path), flags) == 0)    
	{
		return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}

int do_path(char *path, char *pattern)
{
#ifdef
	do_log("do_path...");
#endif

	int flags = 0;
	
	if (fnmatch(pattern, path, flags) == 0)    
	{
		return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}

int do_ls(char * path, char * param)
{	
#ifdef
	do_log("do_ls...");
#endif

	param = param;
	struct stat fileStat;
	char linkPath[PATH_MAX];
	const int timeString_size = 15;
	char timeString[timeString_size]; //Aug  4  14:55\0
	int retVal = 0;

    if(stat(path, &fileStat) != 0)
    {
        return EXIT_FAILURE;
    }
	
	printf("%d %d ", (int) fileStat.st_ino, (int) fileStat.st_blocks);
	
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

    // number of links
    printf(" %d ", (int) fileStat.st_nlink);
	
	// username struct passwd *getpwuid(uid_t uid);
	struct passwd * user = getpwuid(fileStat.st_uid);
	printf("%s ", user->pw_name);
	
	// groupname struct group *getgrgid(gid_t gid);
	struct group * grp = getgrgid(fileStat.st_gid);
	printf("%s ", grp->gr_name);

	// size in bytes
	printf("%d ", (int) fileStat.st_size);
	
	// last modification
	struct tm * modTime;

	modTime = localtime(&fileStat.st_mtime);
	
	if(modTime != NULL)
	{
		retVal = strftime(timeString, timeString_size-2, "%b %d %H:%M", modTime);
		if(retVal == 0)
		{
			return EXIT_FAILURE;
		}
		timeString[retVal] = '\0'; // better safe than sorry...
		printf("%s ", timeString);
	}

	//name
	printf("%s", get_Name(path));

	// -> softlink
	errno = 0;
		
	retVal = readlink(path, linkPath, PATH_MAX-2);
	if (retVal < 0)
	{
		if(errno == EINVAL)
		{
			printf("\n");
			return EXIT_SUCCESS;
		}
		printf("\n%s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	// add terminating 0 , because readlink doesn't do it :(
	linkPath[retVal] = '\0';
	
	printf(" -> %s\n", linkPath);
	
	return EXIT_SUCCESS;
}

int do_type(char * path, char * param)
{
#ifdef
	do_log("do_type...");
#endif

	struct stat mstat;
	if(stat(path, &mstat) == 0)
	{
		if((mstat.st_mode & get_type(param)) != 0)
		{
			return EXIT_SUCCESS;
		}
	}
	
	return EXIT_FAILURE;
}

mode_t get_type(char * param)
{
#ifdef
	do_log("get_type...");
#endif

	if (strcmp(param, type_dir) == 0)
	{
		return S_IFDIR;
	}
	else if(strcmp(param, type_block) == 0)
	{
		return S_IFBLK;
	}
	else if(strcmp(param, type_character) == 0)
	{
		return S_IFCHR;
	}
	else if(strcmp(param, type_pipe) == 0)
	{
		return S_IFIFO;
	}
	else if(strcmp(param, type_file) == 0)
	{
		return S_IFMT;
	}
	else if(strcmp(param, type_link) == 0)
	{
		return S_IFLNK;
	}
	else if(strcmp(param, type_socket) == 0)
	{
		return S_IFSOCK;
	}
	/*
	else if(strcmp(param, type_Door) == 0)
	{
		return 0;
	}
	*/
	else
	{
		return 0;
	}
}

char * get_Name(char * path)
{
#ifdef
	do_log("get_Name...");
#endif

	char * found = path;
	char * temp = path;
	while(temp != NULL)
	{
		if (*temp == '/')
		{
			found = temp;
		}
		temp++;
	}
	return found+1;
}


int myfind(char * path, char ** params)
{
	// count params
	int cnt = 0; // this has to be global
	int retVal = 0;

	while (*params != NULL)
	{
		// @todo find a better way... this will not work
		if ((*params)[0] == '-')
		{
			cnt++;
		}
		params++;
	}

	// create mParamsArray
	sParam * p_mParameters;
	p_mParameters = (sParam *) calloc(cnt + 1, sizeof(sParam));
	// check if successful
	if (p_mParameters == NULL)
	{
		// @todo print error message mem alloc failed
		printf("memory alloc failed in myfind\n");
		return EXIT_FAILURE;
	}

	//parseParams(params);

	retVal = do_dir(path, params);

	free(p_mParameters);

	return retVal;
}

#ifdef LOG_ENABLED

int do_log(char * msg)
{
	if(msg == NULL)
	{
		return EXIT_FAILURE;
	}
	// open file
	FILE * pFile;

	pFile = fopen("myfind.log", "a");

	if(pFile == NULL)
	{
		return EXIT_FAILURE;
	}

	// append msg to file
	fprintf(pf, "%s\n", msg);

	// close file
	fclose(pFile);

	return EXIT_SUCCESS;
}
#endif