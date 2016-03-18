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
 * @details defines the return code for an error from which we cannot recover
 */
#define EXIT_ERROR -1

/**
 * @brief [brief description]
 * @details [long description]
 * 
 * @param path [description]
 * @param params [description]
 * 
 * @return [description]
 */
int myfind(const char * const * params);

#endif
