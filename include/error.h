#ifndef ERROR_H
#define ERROR_H

/*
 * Functions which return result_t must have their return values defined as preprocessor definitions in the header file, where OK must always indicate
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

#endif  // ERROR_H
