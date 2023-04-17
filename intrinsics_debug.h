#define PRINT_MM_FLOAT(size, object)                            \
        printf("_mm%d : \n", size*32);                          \
        for(int idx = 0; idx < size; idx++)                     \
            printf("%5.3f ", ((float *)&object)[idx]);          \
        printf("\n");                                           \
        
#define PRINT_MM_INT(size, object)                              \
        printf("_mm%di %s:\t", size*32, #object);               \
        for (int i = 3; i >= 0; i--)                            \
                for(int j = 3; j >= 0; j--)                     \
                printf("%4x ", ((Uint8*)(&object))[i*4+j]);     \
        printf("\n");                                           \

#define PRINT_U_INT(number_ptr)                                 \
        printf("[%x %x %x %x]", ((u_char *)number_ptr)[0],((u_char *)number_ptr)[0], ((u_char *)number_ptr)[0], ((u_char *)number_ptr)[0]);


inline void printf_m128i(__m128i a)
{
    for (int i = 0; i < 4; i++) printf("%2x ", (Uint8)(((int*)(&a))[i]));
    printf("\n");
}
        // for(int idx = size-1; idx >= 0; idx--)               \
        //     PRINT_U_INT((int *)&object+idx);                 \
        printf("\n");                                           \