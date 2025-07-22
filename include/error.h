#ifndef ERROR_H
#define ERROR_H

/*
 * Functions which return result_t should have their return values defined as preprocessor definitions in the header file, where OK must always
 * indicate success.
 */
typedef int result_t;
#define OK 0

#define THROW_IFEQ(errorcode, variable, erroneousvalue) \
    ({                                                  \
        if ((variable) == (erroneousvalue)) {           \
            return errorcode;                           \
        }                                               \
    })

extern char* g_errstr_custom;
extern char* g_errstr_errno;

#define WAS_ERRNO_USED (*g_errstr_errno != 0)

#endif  // ERROR_H
