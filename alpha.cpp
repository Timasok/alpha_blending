#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "alpha.h"
#include "text_funcs.h"

Img * getImage(const char *source_file_name)
{
    Text_info src = {};
    textCtor(&src, source_file_name);

    printText(&src);

//add check signature
    Img * image = imageCtor(src.buf_length, src.buf);

    // DBG_OUT;
    for (int counter = 0; counter < image->length; counter++)
    {
        image->pixels[counter].r = src.buf[HEADER_LEN+counter*sizeof(int) + 0];
        image->pixels[counter].g = src.buf[HEADER_LEN+counter*sizeof(int) + 1];
        image->pixels[counter].b = src.buf[HEADER_LEN+counter*sizeof(int) + 2];
        image->pixels[counter].a = src.buf[HEADER_LEN+counter*sizeof(int) + 3];
    }

    textDtor(&src);

    return image;
}

Img * imageCtor(size_t buf_len, const char * buf)
{
    const int width_in_header = 18;
    const int height_in_header = 22;

    Img * image = (Img *)calloc(1, sizeof(Img));
    image->length_in_chars = buf_len;
    image->length = (buf_len - (HEADER_LEN + HEADER_LEN%4) )/sizeof(int);
    image->pixels = (pixel *)calloc(image->length, sizeof(pixel));

    image->height = ((int *)(buf+height_in_header))[0];
    image->width  = ((int *)(buf+width_in_header))[0];

    memcpy(image->header, buf, HEADER_LEN);

    return image;
};

Img * imageCopyStruct(Img * src)
{
    Img * image = (Img *)calloc(1, sizeof(Img));
    memcpy(image, src, sizeof(Img));

    image->pixels = (pixel *)calloc(image->length, sizeof(pixel));


    return image;
};


int imagePrint(Img *image)
{
    for (size_t counter = 0; counter < image->length; counter++)
    {
        printf("(%d %d %d %d)\n", 
        image->pixels[counter].r, image->pixels[counter].g, 
        image->pixels[counter].b, image->pixels[counter].a);

    }
    return 0;
};

int imageDtor(Img *image)
{
    free(image->pixels);
    free(image);
    return 0;
};

#ifdef SSE
        #include <emmintrin.h>
        #include <smmintrin.h>
        #include <tmmintrin.h>
#endif

#ifdef DEBUGa
    printf("front_shift = %lu\n"
           "front->length  = %lu\n"
           "result->length = %lu\n",
            front_shift, front->length, result->length);
    int flag = 0;
#endif
#ifdef DEBUGa
            if (!flag)
            {
                printf("first counter = %lu\n", counter);
                flag++;
            }else if (counter - front_shift == front->length)
                printf("last counter = %lu\n", counter);
#endif   

#ifndef SSE
Img * alpha_blend(Img *front, Img *back, int x_shift, int y_shift)
{
    ASSERT(front->length > back->length);

    Img * result = imageCopyStruct(back);
    pixel tmp = {};

    
            // double alpha =  front->pixels[counter].a/255;
            // tmp.r = (unsigned char)((long)front->pixels[counter].r*alpha +  (long)back->pixels[counter].r*(1-alpha));
            // tmp.g = (unsigned char)((long)front->pixels[counter].g*alpha +  (long)back->pixels[counter].g*(1-alpha));
            // tmp.b = (unsigned char)((long)front->pixels[counter].b*alpha +  (long)back->pixels[counter].b*(1-alpha));
            // tmp.a = (unsigned char)((long)front->pixels[counter].a*alpha +  (long)back->pixels[counter].a*(1-alpha));

        // }else
        // {
        // }
#ifdef DEBUG
    printf("x_shift = %lu\n"
           "y_shift = %lu\n",
            x_shift, y_shift);
    sleep(1);
#endif

    printf("back:  (%lu, %lu)\n", back->height, back->width);    
    printf("front: (%lu, %lu)\n", front->height, front->width);   

    int flag_1 = 0;
    int flag_2 = 0;

    for (size_t yi = 0; yi < result->height; yi++)
    {
        for(size_t xi = 0; xi < result->width; xi++)
        {
            size_t back_counter = yi*back->height + xi;
            size_t delta_x = xi - x_shift;
            size_t delta_y = yi - y_shift;
            
            if ( (0 <= delta_x && delta_x <= front->width) && (0 <= delta_y && delta_y <= front->height))
            {   
                size_t front_counter = ((yi-y_shift)*front->height + xi - x_shift);
                // printf("\n");
                if(!flag_1)
                {
                    // printf("bk counter = %lu\n", back_counter);
                    printf("(%lu, %lu)\n", delta_x, delta_y);
                    printf("(%lu, %lu)\n", xi, yi);
                    flag_1++;
                }
#ifdef DEBUG
#endif
            }else
            {
                if(!flag_2 && flag_1)
                {
                    // printf("bk counter = %lu\n", back_counter);
                    printf("(%lu, %lu)\n", delta_x, delta_y);
                    printf("(%lu, %lu)\n", xi, yi);
                    flag_2++;
                }

                tmp.r = back->pixels[back_counter].r;
                tmp.g = back->pixels[back_counter].g;
                tmp.b = back->pixels[back_counter].b;
                tmp.a = back->pixels[back_counter].a;
                
                result->pixels[back_counter] = tmp;
            }

        }
    }

    imageDtor(back);
    imageDtor(front);

    return result;

}


#else  
Img * alpha_blend(Img *front, Img *back, int front_shift)
{
    ASSERT(front->length > back->length);

    Img * result = imageCopyStruct(back);
    pixel tmp = {};

    imageDtor(back);
    imageDtor(front);

    return result;

}

        __m128i fr = _mm_load_si128((__m128i *)&front->pixels[counter]);
        __m128i bk = _mm_load_si128((__m128i *)&back->pixels[counter]);

        __m128i FR = (__m128i) _mm_movpi64_epi64 (*((__m64*)&fr));
        __m128i BK = (__m128i) _mm_movpi64_epi64 (*((__m64*)&bk));

        fr = _mm_cvtepu8_epi16 (fr);
        bk = _mm_cvtepu8_epi16 (bk);

        FR = _mm_cvtepu8_epi16 (FR);                              // сделать воздух более разреженым
        BK = _mm_cvtepu8_epi16 (BK);

const   __m128i moveA = _mm_setr_epi8(15, 14, 15, 14, 15, 14, 15, 14, 6, 5, 6, 5, 6, 5, 6, 5);
const   __m128i _255  = _mm_setr_epi8(0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255);

        __m128i a = _mm_shuffle_epi8(fr, moveA);                  // front.a(0,1)
        __m128i A = _mm_shuffle_epi8(FR, moveA);                  // front.a(2,3)

        fr = _mm_mul_epu32(fr, a);
        FR = _mm_mul_epu32(FR, A);

        a =  _mm_sub_epi32 (_255, a);
        A =  _mm_sub_epi32 (_255, A);

        bk = _mm_mul_epu32(bk, a);
        BK = _mm_mul_epu32(BK, A);

        __m128i sum = _mm_add_epi32(fr, bk);
        __m128i SUM = _mm_add_epi32(FR, BK);

const   __m128i moveSUM = _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 15, 13, 11, 9, 7, 5, 3, 1);

        sum = _mm_shuffle_epi8(sum, moveSUM);
        SUM = _mm_shuffle_epi8(SUM, moveSUM);

        __m128i color = (__m128i)_mm_move_ss((__m128)sum, (__m128)SUM); 

        // tmp.r = back->pixels[counter].r + front->pixels[counter].r;
        // tmp.g = back->pixels[counter].g + front->pixels[counter].g;
        // tmp.b = back->pixels[counter].b + front->pixels[counter].b;
        // tmp.a = back->pixels[counter].a + front->pixels[counter].a;
#endif

int saveAsBMP(Img *result, const char *result_file_name)
{
    int buf_len = result->length_in_chars;

    unsigned char * buf = (unsigned char *)calloc(buf_len, sizeof(unsigned char));

    memcpy(buf, result->header, HEADER_LEN);

    // buf[0] = 'B';
    // buf[1] = 'M';

    // *((int *)(buf + 2)) = buf_len;
    
    for (size_t counter = 0; counter < result->length; counter++)
    {
        buf[HEADER_LEN+counter*sizeof(int) + 0] = result->pixels[counter].r;
        buf[HEADER_LEN+counter*sizeof(int) + 1] = result->pixels[counter].g;
        buf[HEADER_LEN+counter*sizeof(int) + 2] = result->pixels[counter].b;
        buf[HEADER_LEN+counter*sizeof(int) + 3] = result->pixels[counter].a;
    }

    imageDtor(result);

    FILE * result_file = fopen(result_file_name, "wb");
    fwrite(buf, sizeof(unsigned char), buf_len, result_file);
    fclose(result_file);
    free(buf);

    return 0;
}