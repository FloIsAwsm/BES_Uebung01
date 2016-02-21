#include <string.h>
#include "myfind.h"

/* Constants */
const char * command_ls = "-ls";
const char * command_print = "-print";
const char * err_msg_unknown_pred = "unknown predicate: ";
const char * err_msg_missing_arg = "missing argument to ";
const char * command_user = "-user";
const char * command_name = "-name";
const char * command_type = "-type";
const char* upperDir = "..";
const char* currentDir = ".";

static bool containsPrint = false;

// array of parameter functions
// no idea if this would compile ;)
static sParam m_Parameters [MAX_PARAMS];

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
		else
		{
			// print error message
			printf("%s%s\n", err_msg_unknown_pred, (*params));
			return EXIT_FAILURE;
		}
		index++;
	}
	m_Parameters[index].func = NULL;
	return EXIT_SUCCESS;
}

int do_print(char * path, char * param/* = NULL */)
{
	if(param == NULL)
	{
	}
	printf("%s\n", path);
	return EXIT_SUCCESS;
}
