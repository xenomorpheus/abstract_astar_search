#ifndef _types_h_
#define _types_h_

/**
 * @file
 *
 * @brief In this module we define globally used types within our codebase.
 */

/*
 * stdc libraries
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h> /* for size_t on *nix platforms */

/**
 * @def
 * @brief Used to clearly label interal functions.
 */
#define internal static

/**
 * @def
 * @brief Used to clearly label local persistence within a function.
 */
#define local_persist static

/**
 * @def
 * @brief Used to clearly label a variable as global.
 */
#define global_variable static

/**
 * @def
 * @brief A null pointer type.
 */
#define null_ptr_t (void *) 0

#endif /* _types_h_ */
