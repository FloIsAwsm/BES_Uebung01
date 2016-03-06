/**
 * @file myfind.c
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
 * @todo implement command functions
 * @todo complete documentation
 * @todo comment code
 * @todo use const values in function declarations if possible
 */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "myfind.h"

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
	if(param[0] == '-')
	{
		return false;
	}
	return true;
}

int do_dir(char * dir, char ** params)
{
	static bool firstEntry = true;
	if (firstEntry)
	{
		if(parseParams(params) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}
	}

	DIR * pdir;
	struct dirent * item;

	if(!(pdir = opendir(dir)))
	{
		// cannot open dir
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
				handleParams(path);
				do_dir(path, params); // what if we return with EXIT_FAILURE
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
		else if(strcmp((*params), command_ls) == 0)
		{
			m_Parameters[index].func = &do_ls;
			m_Parameters[index].param = NULL;
			containsPrint = true;	
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

int do_ls(char * path, char * param)
{
	param = param;
	struct stat fileStat;
	char linkPath[PATH_MAX];
	const int timeString_size = 14;
	char timeString[timeString_size]; //Aug  4  14:55\0
	int retVal = 0;

    if(stat(path,&fileStat) != 0)
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
		retVal = strftime(timeString, timeString_size-1, "%b %d %H:%M", modTime);
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
		
	retVal = readlink(path, linkPath, PATH_MAX-1);
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
