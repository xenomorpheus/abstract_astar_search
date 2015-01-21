#ifndef _logging_h_
#define _logging_h_

/**
 * @file
 *
 * @brief Defines some very simple logging calls, that currently just log to
 * stderr.
 *
 * Also defines a very very simple check() macro -- which should most likely
 * be moved into a debugging module.
 */

/*
 * stdc libraries
 */
#include <stdio.h>

/*****************************************************************************
 *
 * External logging functions that may be called by clients
 *
 ****************************************************************************/

void
logging_set_log_file(FILE* log);

FILE*
logging_get_log_file(void);

void
logging_log(FILE* log_file, const char* format, ...);


/*****************************************************************************
 *
 * Logging & debug macros
 *
 ****************************************************************************/

#define debug_out(M, ...) logging_log(logging_get_log_file(), M, ##__VA_ARGS__)


#define log_error(M, ...) debug_out("[error] <%s:%d> " M "\n", \
                                    __FILE__, __LINE__, ##__VA_ARGS__)

#define log_warn(M, ...) debug_out("[warn] <%s:%d> " M "\n",\
                                   __FILE__, __LINE__, ##__VA_ARGS__)

#define log_info(M, ...) debug_out("[info] <%s:%d> " M "\n", \
                                   __FILE__, __LINE__, ##__VA_ARGS__)


/*
 * for all intensive purposes this acts like a "try" and will jump to error:
 * on failure
 */
#define check(A, M, ...) if(!(A)) { log_error(M, ##__VA_ARGS__); goto error; }

#endif /* _logging_h_ */
