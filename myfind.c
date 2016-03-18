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
//#define LOG_ENABLED
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
/* environment */
const char* env_posixly_correct = "POSIXLY_CORRECT";

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
 * @brief global variable for the application name
 */
static char * app_name;

/**
 * @brief checks if the string is a valid path
 * @details checks if the first char of the string is a '-',
 * if it is we assume it is a parameter,
 * if not we assume it is a path
 * 
 * @param param c-string that is either a command or a path
 * @return false if param is a command, true otherwise
 */
static bool IsValidPath(char * param);

/**
 * @brief recursive function to be called on every directory
 * @details this function is called on every directory. It opens the
 * directory and calls the handleParams function for every directory
 * in it.
 * 
 * @param dir cstring containing the name of the directory
 * @param params array of cstrings containing the commands and parameters
 * 
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int do_dir(char * dir, char ** params);

/**
 * @brief checks if the parameters are correct and makes them easier to handle
 * @details checks the validity of our the commands and fills our parameter array m_Parameters
 * 
 * @param params the parameters we have to process
 * @return EXIT_SUCCESS if all commands are understood, EXIT_FAILURE otherwise 
 */
static int parseParams(const char * path, const char * const * params); // @todo

/**
 * @brief calls the parameter functions on a path
 * @details calls all parameter functions like "-name <name>"
 * in our parameter list on every directoy and file 
 * 
 * @param path the path we have to use the parameters on
 * @return EXIT_FAILURE or EXIT_SUCCESS
 */
//static int handleParams(char * path);

/**
 * @brief prints a path in ls format
 * @details [long description]
 * 
 * @param path the path of the file or directory to be printed
 * @param param always NULL
 * 
 * @return EXIT_FAILURE on error, EXIT_SUCCESS otherwise
 */
static int do_ls(const char *path, const char *pattern);

/**
 * @brief function which matches a given path with the pattern
 * @details match the pattern with the path
 * 
 * @param path directory path
 * @param pattern to search for
 * 
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int do_name(const char *path, const char *pattern);

/**
 * @brief checks a file if the owner of it equals the entered user
 * @details function checks the file if the entered user equals the owner of the file
 * 
 * @param path path including filename which needs to be checked
 * @param user username or UID
 * 
 * @return EXIT_SUCCESS if the file or directory matches the user in param, EXIT_FAILURE otherwise
 */
static int do_user(const char * path, const  char * param);

/**
 * @brief checks a file if it has a valid owner
 * @details function checks the file if it has a valid owner
 * 
 * @param path path including filename which needs to be checked
 * @param param always NULL
 * 
 * @return returns EXIT_SUCCESS, if the file has no valid owner, EXIT_FAILURE otherwise
 */
static int do_nouser(const char * path, const char * param);

/**
 * @brief path with pattern matching
 * @details funtion to check if given pattern is in path and return complete path
 * 
 * @param path to search for
 * @param pattern to matched with the given path
 * 
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int do_path(const char * path, const char * pattern);

/**
 * @brief prints the path on the console
 * @details prints the path on the console
 * 
 * @param path the path to be printed
 * @param param always NULL
 * 
 * @return always EXIT_SUCCESS
 */
static int do_print(const char * path, const char * param);

/**
 * @brief determines if a file is of a specific type
 * @details [long description]
 * 
 * @param path file or directory to be tested
 * @param type a string containing a letter for a specific type
 * 
 * @return EXIT_SUCCESS if the file is of that type, EXIT_FAILURE otherwise
 */
static int do_type(const char * path, const char * type);

/**
 * @brief converts a c string with one letter into a type bitmask
 * @details converts a c string into a bitmask for it's defined type
 * 
 * @param param c string containing the type specifier
 * @return the bitmask defined by param, 0 on failure
 */
static mode_t get_type(const char * param);

/**
 * @brief converts a path into a filename
 * @details returns the filename of the current path
 * 
 * @param path the path with the filename at the end
 * @return a pointer to the start of the filename
 */
static char * get_Name(const char * path);

/**
 * @brief prints the correct useage of this module
 */
static void printusage(void);


static bool IsValidPath(const char * param)
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


static int do_dir(const char * dir, const char * const * params)
{
	DIR * pdir;		
	struct dirent * item;

	parseParams(dir, params);
	
	errno = 0;
	pdir = opendir(dir);
	if(pdir == NULL)
	{
		/* cannot open dir */
		fprintf(stderr, "%s: '%s': %s\n", app_name, dir, strerror(errno));
		return EXIT_FAILURE;
	}

	errno = 0;
	while ((item = readdir(pdir)) != NULL)
	{
		int size = strlen(dir) + strlen(item->d_name) + 2; // terminating null + seperating '/'
		char * path = (char *) malloc(size, sizeof(char));
		if (path == NULL)
		{
			fprintf(stderr, "%s: %s\n", app_name, strerror(ENOMEM)); //@todo maybe errno
			// @todo cleanup
			return EXIT_FAILURE;
		}
		int len = snprintf(path, size, "%s/%s", dir, item->d_name);
		path[len] = 0;
		if(item->d_type == DT_DIR)
		{
			if (!(strcmp(item->d_name, currentDir) == 0 || strcmp(item->d_name, upperDir) == 0))
			{
				do_dir(path, params); // @todo what if we return with EXIT_FAILURE
			}
		}
		else
		{
			parseParams(path, params);
		}
		free(path);
		errno = 0;
	}
	if(errno != 0)
	{
		fprintf(stderr, "%s: %s\n", app_name, strerror(errno));
		// try to close directory
		errno = 0;
		if(closedir(dir) != 0)
		{
			fprintf(stderr, "%s: %s\n", app_name, strerror(errno));
			return EXIT_FAILURE;
		}
		return EXIT_FAILURE;
	}
	errno = 0;
	if(closedir(pdir) != 0)
	{
		fprintf(stderr, "%s: %s\n", app_name, strerror(errno));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/*
static int handleParams(const char * path, const char * const * params)
{
	for(size_t i = 0; params != NULL; ++i)
	{
		if(do_params(params[i], params[(i + 1)]) == EXIT_FAILURE) //@todo need path
		{
			return EXIT_FAILURE;
		}
	}
	if(!containsPrint)
	{
		do_print(path, NULL);
	}
#ifdef LOG_ENABLED
	do_log("exiting handleParams...");
#endif
	return EXIT_SUCCESS;
}
*/

static int parseParams(const char * path, const char * const * params)
{
#ifdef LOG_ENABLED
	do_log("parseParams...");
#endif
	bool containsPrint = false;
	int retVal = EXIT_SUCCESS;
	int index = 0;
	for(size_t i = 0; (params[i] != NULL) && (retVal == EXIT_SUCCESS); ++i)
	{
		if(strcmp(params[i], command_print) == 0)
		{
			do_print(path);
			containsPrint = true;
		}
		else if(strcmp(params[i], command_ls) == 0)
		{
			do_ls(path);
			containsPrint = true;
		}
		else if(strcmp(params[i], command_path) == 0)
		{
			++i;
			if (params[i] != NULL)
			{
				retVal = do_path(path, params[i])
			}
			else
			{
				fprintf(stderr, "%s: %s'%s'\n", app_name, err_msg_missing_arg, command_path);
				printusage();
				return EXIT_FAILURE;
			}
		}		
		else if(strcmp(params[i], command_name) == 0)
		{
			++i;
			//m_Parameters[index].func = &do_name;
			if (params[i] != NULL)
			{
				retVal = do_name(path, params[i])
			}
			else
			{
				fprintf(stderr, "%s: %s'%s'\n", app_name, err_msg_missing_arg, command_name);
				printusage();
				return EXIT_FAILURE;
			}
		}
		else if(strcmp(params[i], command_user) == 0)
		{
			++i;
			//m_Parameters[index].func = &do_user;
			if (params[i] != NULL)
			{
				retVal = do_user(path, params[i]);
			}
			else
			{
				// print error message
				fprintf(stderr, "%s: %s '%s'\n", app_name, err_msg_missing_arg, command_user);
				printusage();
				return EXIT_FAILURE;
			}
		}
		else if(strcmp(params[i], command_nouser) == 0)
		{
			retVal = do_nouser(path);
		}
		else if(strcmp(params[i], command_type) == 0)
		{
			++i;
			if(params[i] != NULL)
			{
				if (strlen(params[i]) != 1)
				{
					fprintf(stderr, "%s: Arguments to type should only contain one letter\n", app_name);
					printusage();
					return EXIT_FAILURE;
				}
				if (get_type(params[i]) == 0)
				{
					fprintf(stderr, "%s: unknown argument to -type: %s", app_name, params[i]);
					printusage();
					return EXIT_FAILURE;
				}
				retVal = do_type(path, params[i]);
			}
			else
			{
				fprintf(stderr, "%s: %s'%s'\n", app_name, err_msg_missing_arg, command_type);
				printusage();
				return EXIT_FAILURE;
			}
			++i;
		}
		else
		{
			/* print error message */
			if(params[i][0] == '-')
			{
				fprintf(stderr, "%s%s\n", err_msg_unknown_pred, (*params));
			}
			else
			{
				fprintf(stderr, "paths must exceed the expression\n");
			}
			printusage();
			return EXIT_FAILURE;
		}
	}
	if(!containsPrint)
	{
		do_print(path);
	}
	return EXIT_SUCCESS;
}

static int do_print(const char * path, const char * param)
{
	param = param;
	fprintf(stdout, "%s\n", path);
	return EXIT_SUCCESS;
}


static int do_user(const char * path, const char * param)
{	
#ifdef LOG_ENABLED
	do_log("do_user...");
#endif

	struct stat buf;
	uid_t uid;
	uid_t item_uid;
	//struct passwd * get_uid;
	char * pEnd;

	// first check if parameters are correct
	struct passwd * user = getpwnam(param);
	if (user == NULL)
	{
		char * pEnd = 0;
		uid_t uid = strtol(params[i], &pEnd, 10);
		errno = 0;
		if ((size_t) (pEnd - params[i]) == strlen(params[i]) && (user = getpwuid(uid)) != NULL)
		{
			//m_Parameters[index].param = *(params);
			//do_user() @todo
			errno = 0;
			if(stat(path, &buf) < 0)
			{
				fprintf(stderr, "%s: '%s': %s\n", app_name, path, strerror(errno));
				return EXIT_FAILURE;
			}
		}
		else
		{
			if(errno == ENOENT)
			{
				fprintf(stderr, "%s: '%s' is not the name of a known user\n", app_name, param);
				printusage();
				return EXIT_FAILURE;
			}
			else
			{
				fprintf(stderr, "%s: %s: %s\n", app_name, strerror(errno), param);
				return EXIT_FAILURE;
			}
		}
	}


	if (user->pw_uid == buf.st_uid)
	{
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}


	errno = 0;
	if(stat(path, &buf) < 0)
	{
		fprintf(stderr, "%s: '%s': %s\n", app_name, path, strerror(errno));
		return EXIT_FAILURE;
	}

	// get uid of current path
	item_uid = buf.st_uid;
	// assume param is a username
	get_uid = getpwnam(param);
	if(get_uid != NULL) /* no existing user with entered username */
	{
		uid = get_uid->pw_uid;	/* parameter is a username */
	}
	else
	{
		uid = (uid_t) strtol(param, &pEnd, 10); /* parameter is an UID */
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

static int do_nouser(const char * path, const char * param)
{
#ifdef LOG_ENABLED
	do_log("do_nouser...");
#endif

	struct passwd *get_uid;
	struct stat buf;
	int item_uid;

	// unused parameter warning
	param = param;

	if(stat(path, &buf) < 0)
	{
		fprintf(stderr, "%s: '%s': %s\n", app_name, path, strerror(errno));
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

static int do_name(const char *path, const char *pattern)
{
#ifdef LOG_ENABLED
	do_log("do_name...");
#endif

	int flags = 0;
	
	if (fnmatch(pattern, get_Name(path), flags) == 0)    
	{
		return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}

int do_path(const char *path, const char *pattern)
{
#ifdef LOG_ENABLED
	do_log("do_path...");
#endif

	int flags = 0;
	
	if (fnmatch(pattern, path, flags) == 0)    
	{
		return EXIT_SUCCESS;
    }
    
    return EXIT_FAILURE;
}

static int do_ls(const char * path, const char * param)
{	
#ifdef LOG_ENABLED
	do_log("do_ls...");
#endif

	param = param;
	unsigned long blocks;
	struct stat fileStat;
	char linkPath[PATH_MAX];
	const int timeString_size = 15;
	char timeString[timeString_size]; //Aug  4  14:55\0
	int retVal = 0;

	errno = 0;
    if(lstat(path, &fileStat) != 0)
    {
    	fprintf(stderr, "%s: '%s': %s\n", app_name, path, strerror(errno));
        return EXIT_FAILURE;
    }
	
	blocks = (unsigned long) fileStat.st_blocks;
	if (getenv(env_posixly_correct) == NULL)
	{
		blocks /= 2;
	}
	printf("%6lu %4lu ", (unsigned long) fileStat.st_ino, (unsigned long) (fileStat.st_blocks / 2));
	
	printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    if ((fileStat.st_mode & S_IXUSR) && !(fileStat.st_mode & S_ISUID))
    {
    	printf("x");
    }
    else if((fileStat.st_mode & S_IXUSR) && (fileStat.st_mode & S_ISUID))
    {
    	printf("s");
    }
    else if(!(fileStat.st_mode & S_IXUSR) && (fileStat.st_mode & S_ISUID))
    {
    	printf("S");
    }
    else
    {
    	printf("-");
    }
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    if ((fileStat.st_mode & S_IXGRP) && !(fileStat.st_mode & S_ISGID))
    {
    	printf("x");
    }
    else if((fileStat.st_mode & S_IXGRP) && (fileStat.st_mode & S_ISGID))
    {
    	printf("s");
    }
    else if(!(fileStat.st_mode & S_IXGRP) && (fileStat.st_mode & S_ISGID))
    {
    	printf("S");
    }
    else
    {
    	printf("-");
    }
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    if ((fileStat.st_mode & S_IXOTH) && !(fileStat.st_mode & S_ISVTX))
    {
    	printf("x");
    }
    else if((fileStat.st_mode & S_IXOTH) && (fileStat.st_mode & S_ISVTX))
    {
    	printf("t");
    }
    else if(!(fileStat.st_mode & S_IXOTH) && (fileStat.st_mode & S_ISVTX))
    {
    	printf("T");
    }
    else
    {
    	printf("-");
    }

    // number of links
    printf(" %3d ", (int) fileStat.st_nlink);
	
	// username struct passwd *getpwuid(uid_t uid);
	struct passwd * user = getpwuid(fileStat.st_uid);
	if(user != NULL)
	{
		printf("%-8s ", user->pw_name);
	}
	else
	{
		printf("%-8lu ", (unsigned long) fileStat.st_uid);
	}
	
	// groupname struct group *getgrgid(gid_t gid);
	struct group * grp = getgrgid(fileStat.st_gid);
	if(grp != NULL)
	{
		printf("%-8s ", grp->gr_name);
	}
	else
	{
		printf("%-8lu ", (unsigned long) fileStat.st_gid);
	}

	// size in bytes
	printf("%8lu ", (unsigned long) fileStat.st_size);
	
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
	printf("%s", path);

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
	linkPath[retVal] = 0;
	
	printf(" -> %s\n", linkPath);
	
	return EXIT_SUCCESS;
}

static int do_type(const char * path, const char * param)
{
#ifdef LOG_ENABLED
	do_log("do_type...");
#endif

	struct stat mstat;
	errno = 0;
	if(lstat(path, &mstat) == 0)
	{
		if(((mstat.st_mode & S_IFMT) == get_type(param)) != 0)
		{
			return EXIT_SUCCESS;
		}
	}
	else
	{
		fprintf(stderr, "%s: %s: %s\n", app_name, strerror(errno), path);
	}
	
	return EXIT_FAILURE;
}

static mode_t get_type(const char * param)
{
#ifdef LOG_ENABLED
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
		return S_IFREG;
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

static char * get_Name(const char * path)
{
#ifdef LOG_ENABLED
	do_log("get_Name...");
#endif

	char * found = path;
	char * temp = path;
	while(temp != NULL && *temp != '\0')
	{
		if (*temp == '/')
		{
			found = temp;
		}
		temp++;
	}
	if(found != NULL && found != NULL)
	{
		return found+1;
	}
	else
	{
		return path;
	}
}


int myfind(const char * const * params)
{
	// write app_name
	app_name = params[0];

	char * path = ".";
	// @todo if we do a validity check we do it here
	if(IsValidPath(params[1]))
	{
		path = params[1]
	}
	
	// find out if file or directory
	struct stat mstat;
	errno = 0;
	if(stat(path, &mstat) != 0)
	{
		fprintf(stderr, "%s: '%s': %s\n", app_name, path, strerror(errno));
		return EXIT_FAILURE;
	}
	if(!S_ISDIR(mstat.st_mode))
	{
		handleParams(path, params);
		return EXIT_SUCCESS;
	}

	return do_dir(path, params);
}

static void printusage(void) {
	/*
	printf("\nUsage: %s <path> [action]\n\n"\
		"actions:\n"\
		" -user <name>|<uid>\n"\
		" -nouser\n"\
		" -name <pattern>\n"\
		" -path <pattern>\n"\
		" -type [bcdpfls]\n"\
		" -print\n"\
		" -ls\n\n", app_name);
		*/
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
	fprintf(pFile, "%s\n", msg);

	// close file
	fclose(pFile);

	return EXIT_SUCCESS;
}
#endif