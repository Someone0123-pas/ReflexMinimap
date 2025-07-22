#ifndef ERROR_H
#define ERROR_H

#include <errno.h>
#include <stdio.h>

/*
 * Functions which return result_t should have their return values defined as preprocessor definitions in the header file, where OK must always indicate
 * success.
 */
typedef int result_t;
#define OK 0

#define THROW_IFEQ(errorcode, variable, erroneousvalue) \
    ({                                                  \
        if ((variable) == (erroneousvalue)) {           \
            return errorcode;                           \
        }                                               \
    })

#define NO_IMPL_EXIT(functionname)                                         \
    ({                                                                     \
        fprintf(stderr, "%s returned an unknown result.\n", functionname); \
        return 1;                                                          \
    })

extern char* g_errorstr[0x100];

#endif  // ERROR_H
