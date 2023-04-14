#define PRINT_MM_FLOAT(size, object)                            \
        printf("_mm%d : \n", size*32);                          \
        for(int idx = 0; idx < size; idx++)                     \
            printf("%5.3f ", ((float *)&object)[idx]);          \
        printf("\n");                                           \
        
#define PRINT_MM_INT(size, object)                              \
        printf("_mm%di : \n", size*32);                         \
        for(int idx = 0; idx < size; idx++)                     \
            printf("%d ", ((int *)&object)[idx]);               \
        printf("\n");                                           \