#ifndef _minunit_h
#define _minunit_h

#include "logging.h"

/*
 * stdc libraries
 */
#include <stdio.h>
#include <stdlib.h>

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message) if (!(test)) { log_error("%s", message); return message; }
#define mu_run_test(test) log_info("\ttest: <%s>", #test); \
    message = test(); tests_run++; if (message) return message;

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
        argc = 1; \
        log_info("test_suite: <%s>", argv[0]);\
        char *result = name();\
        if (result != 0) {\
            log_info("failed: %s", result);\
        }\
        else {\
            log_info("all tests passed!");\
        }\
        log_info("tests run: %d", tests_run);\
        exit(result != 0);\
    }

int tests_run;

#endif
