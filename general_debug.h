#ifndef GEN_DEBUG_H
#define GEN_DEBUG_H

// #ifdef DEBUG_MODE
// #define DBG_OUT printf("DBG at %s at %s(%d)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__)
// #endif
// #ifndef DEBUG_MODE
// #define DBG_OUT do{}while(0)
// #endif
#ifdef DEBUG_MODE
#define DBG_OUT fprintf(stderr, "Compiled nicely -line: %d file: %s func: %s\n", \
                        __LINE__, __FILE__, __FUNCTION__)

#define PARSE_ERROR(cpuPtr, condition, error_specifier) \
    cpuPtr->code_of_error |= ((condition)&error_specifier);

#endif
#ifndef DEBUG_MODE
#endif


#define DBG_OUT printf("DBG at %s at %s(%d)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__)

// #define PRINT_ERROR_CONSOLE(error_specifier)                                        \
//         fprintf(stderr, "%s", #error_specifier)                                     \
        
#define PRINT_ERROR_CONSOLE(...)                                                  \
        fprintf(stderr, "\e[0;31m");                                              \
        fprintf(stderr, __VA_ARGS__);                                             \
        fprintf(stderr, "\e[0m\n")                                                \

#define ASSERT(condition)                                                       \
    do{                                                                         \
        if (!condition)                                                         \
        {                                                                       \
            fprintf(stderr, "\e[0;31m%s failed at %s at %s(%d)\e[0m\n",         \
                     #condition, __PRETTY_FUNCTION__, __FILE__, __LINE__);      \
        }                                                                       \
    } while(0)                                                                  \

#define ASSERT_ASS(condition)                                           \
    do{                                                                 \
        if (!condition)                                                 \
        {                                                               \
            fprintf(stderr, "%s failed: file: %s func: %s line: %d\n",  \
                     #condition, __FILE__, __FUNCTION__, __LINE__);     \
            return -1;                                                  \
        }                                                               \
    } while(0)                                                          \

#endif