/*
 * logging.c:
 *
 * In this module we implement very simple log to stderr logging.
 */

#include <time.h>
#include <stdio.h>

// Custom
#include <types.h>

/*
 * bstring library
 */
#include <bstrlib.h>
#include <bstraux.h>


/*
 * the log file that we wish to log too
 */
global_variable FILE* log_file_global = null_ptr_t;

/*
 * logging_set_log_file:
 *
 * Sets the log file handle to the one provided. Does no cleanup / closing of
 * the file handle at any stage.
 */
void
logging_set_log_file(FILE* log_file) {
    log_file_global = log_file;
}

/*
 * logging_get_log_file:
 *
 * Returns a handle to the file set by logging_set_log_file("foo.log") or
 * returns stderr if this has not been set by a user.
 */
FILE*
logging_get_log_file() {
    return log_file_global != null_ptr_t ? log_file_global : stderr;
}

/*
 * logging_log:
 *
 * Log a message to file and stdout with a timestamp.
 */
void
logging_log(FILE* log_file, const char* format, ...) {
    va_list args;
    time_t now = time(NULL);
    bstring time_stamp = bStrfTime("[%H:%M:%S]", gmtime(&now));
    va_start(args, format);
    fprintf(log_file, "%s ", time_stamp->data);
    vfprintf(log_file, format, args);
    bdestroy(time_stamp);
    va_end(args);
}

