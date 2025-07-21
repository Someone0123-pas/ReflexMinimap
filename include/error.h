#ifndef ERROR_H
#define ERROR_H

#include <errno.h>
#include <stdio.h>

/*
 * Functions which return result_t must have their return values defined as preprocessor definitions in the header file, where OK must always indicate
 * success.
 */
typedef int result_t;
#define OK 0

/*
 * Should only be used by main()
 */
#define EXIT_ERR(message)      \
    ({                         \
        perror(message);       \
        int cur_errno = errno; \
        socket_close();        \
        return cur_errno;      \
    })

#define THROW_IFEQ(errorcode, variable, erroneousvalue) \
    ({                                                  \
        if ((variable) == (erroneousvalue)) {           \
            return errorcode;                           \
        }                                               \
    })

#define NO_IMPL_EXIT(functionname) ({ fprintf(stderr, "%s returned an unknown result.\n", functionname); })

#endif  // ERROR_H
