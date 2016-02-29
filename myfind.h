/**
 * @file myfind.h
 * 
 * Beispiel 1
 * 
 * @author Florian Froestl 	<florian.froestl@technikum-wien.at>
 * @author David Boisits	<david.boisits@technikum-wien.at>
 * @author Markus Diewald	<markus.diewald@technikum-wien.at>
 * 
 * @date 2016/02/22
 * 
 * @version 100
 * 
 * @todo modify includes (move to myfind.c if possible)
 */

#ifndef MYFIND_H
#define MYFIND_H

#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h> /* PATH_MAX */
#include <unistd.h>
#include <fnmatch.h>
#include <sys/stat.h>

/**
 * @brief global variable for the application name
 */
extern char * app_name;

/**
 * @brief the maximal amout of parameters
 * @details the maximal amount of parameters that can be passed to the program
 * 
 * @todo this is an arbitrary number at the moment...
 */
#define MAX_PARAMS 100

/**
 * @brief success return code
 * @detais defines the return code for a successful operation
 */
#define EXIT_SUCCESS 0

/**
 * @brief failure return code
 * @detais defines the return code for a failed operation or an error
 */
#define EXIT_FAILURE 1

/**
 * @brief checks if the string is a valid path
 * @details checks if the first char of the string is a '-',
 * if it is we assume it is a parameter,
 * if not we assume it is a path
 * 
 * @param param c-string that is either a command or a path
 * @return false if param is a command, true otherwise
 */
bool IsValidPath(char * param);

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
int do_dir(char * dir, char ** params);

/**
 * @brief [brief description] 
 * @details [long description]
 * 
 * @param file [description]
 * @param params [description]
 * 
 * @return [description]
 */
int do_file(char * file, char ** params);

int do_name(const char *pattern, const char *path, int flags);

int do_ls(char *path);

#endif
