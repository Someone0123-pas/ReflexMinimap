#ifndef ERROR_H
#define ERROR_H

/*
 * Functions which return result_t must have their return values defined as preprocessor definitions in the header file, where 0 must always indicate
 * success.
 */
typedef int result_t;
#define OK 0

#define THROW_IFEQ(result, variable, erroneous) \
    ({                                          \
        if ((variable) == (erroneous)) {        \
            return result;                      \
        }                                       \
    })

#endif  // ERROR_H
