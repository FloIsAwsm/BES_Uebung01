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
 */

#ifndef MYFIND_H
#define MYFIND_H

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
 * @brief error return code
 * @details defines the return code for an error from which we cannot recover.
 * This is always the case if the input arguments are wrong. (e.g.: ./myfind . -type xy)
 */
#define EXIT_ERROR -1

/**
 * @brief performs the find operation
 * @details this is the only entry point for this module. It is given an cstring array 
 * containing the application name, possibly a path, commands and their parameters.
 * 
 * It can currently handle the following parameters:
 * -print
 * -ls
 * -type [bcdpfls]
 * -name <pattern>
 * -path <pattern>
 * -user <name>|<uid>
 * -nouser
 * 
 * @param params a cstring array in the following format: [application name][path](optional)[parameter][...][NULL]
 * 
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int myfind(const char * const * params);

#endif
