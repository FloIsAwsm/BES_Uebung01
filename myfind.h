/**
 * TODO file header
 * das wäre eine basis wie wir unser projekt anlegen können
 * wir haben mehr als genug funktionen die man alleine entwickeln kann
 * vorallem aber auch gleichzeitig...
 * Es sollte dabei auch kein problem sein auf einer file zu arbeiten...
 * genaueres habe ich aber erst wenn ich mich selber in den git workflow rein gearbeitet habe...
 * Ihr könnt entweder in dieser file kommentieren und dann pushen oder (falls ihr es am handy schon habt)
 * per Telegram schreiben... es gibt auch eine telegram desktop app für windows 10
 */

#ifndef MYFIND_H
#define MYFIND_H

// there is a define similar to that, that is used by most system calls but i'm not sure what it's name is
// for now just use MAx_PATH
#define MAX_PATH 2048

// right now this is an arbitrary number
// we can define this later on
// we can handle MAX_PARAMS-1 parameters
// 
#define MAX_PARAMS 100

#define EXIT_SUCCESS 0

#define EXIT_FAILURE 1

/**
 * @brief checks if the string is a valid path
 * @details checks if the first char of the string is a '-',
 * if it is we assume it is a parameter,
 * if not we assume it is a path... It is also possible to
 * check if it is a valid path and exit the program if not...
 * 
 * @param param string that might be a directory or file
 * @return true if the path is valid, false otherwise
 */
bool IsValidPath(const char * param);

int do_dir(char * dir, const char * const * params);

int do_file(char * file, const char * const * params);

/**
 * @brief checks if the parameters are correct and makes them easier to handle
 * @details checks whether it is possible to find a file with those parameters 
 * (e.g.: searching 2 different names would not lead to a result) and brings them into a form
 * that is easier to handle later on... it also fills our parameter array
 * 
 * @param params the parameters we have to process
 * @return in this iteration: EXIT_SUCCESS if a search is usefull, EXIT_FAILURE otherwise 
 */
int parseParams(const char * const * params);

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
 * @details TODO
 * 
 * @param c path
 * @param r specific parameter if needed
 * 
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
typedef int (*paramFunc) (char *, char *);


typedef struct
{
	paramFunc* func;
	char * param;
} sParam;

// array of parameter functions
// no idea if this would compile ;)
sParam m_Parameters [MAX_PARAMS];


/* Those are our parameter functions... we can split the implementatons on those */
int do_ls(char * path, char * param = NULL);

int do_name(char * path, char * name);

int do_user(char * path, char * user);

int do_userid(char * path, char * uid);

int do_nouser(char * path, , char * param = NULL);

int do_path(char * path, char * pattern);

int do_print(char * path, char * param = NULL);

int do_type(char * path, char * type);

#endif