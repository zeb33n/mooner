#ifndef UTIL_ATTRIBUTE_H
#define UTIL_ATTRIBUTE_H

#define __noreturn           __attribute__((noreturn))
#define __optimize(level)    __attribute__((optimize(level)))
#define __const_func         __attribute__((const))
#define __pure_func          __attribute__((pure))
#define __warn_unused_result __attribute__((warn_unused_result))

#endif
