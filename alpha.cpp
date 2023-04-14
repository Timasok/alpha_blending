#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "alpha.h"
#include "text_funcs.h"

static int setPixel(void *src, void *dest)
{
    ASSERT_ASS(dest); 
    ASSERT_ASS(src);

    for(int idx = 0; idx < 4; idx++)
    {
        ((u_char *)dest)[idx] = ((u_char *)src)[idx];   
    }

    return 0;
}

Img * getImage(const char *source_file_name)
{
    Text_info src = {};
    textCtor(&src, source_file_name);

    printText(&src);
    
    Img * image = imageCtor(src.buf_length, src.buf);


    u_char color = 0;
    u_int pixel = 0;
    u_char * source = ((u_char *)(&src.buf[HEADER_LEN]));

    size_t counter = image->length - 1;
    for(; counter > 0; counter--)
    {
        pixel = 0;

        // printf("RGB: ( ");
        for(int idx = 0; idx < image->color_size; idx++)
        {
            color = source[idx];
            // printf("%2x ", color);
            pixel+=color<<idx*BYTE;
        }
        // printf(")\n");

        image->pixels[counter] = pixel;

        source+=image->color_size;

        //printf("%d\n", HEADER_LEN + counter*image->color_size);
    }
    
    textDtor(&src);

    return image;
}

static const int width_in_header = 18;
static const int height_in_header = 22;
static const int color_size_in_header = 28;
static const int image_size_in_header = 32;

Img * imageCtor(size_t buf_len, const char * buf)
{

    Img * image = (Img *)calloc(1, sizeof(Img));
    image->length_in_chars = buf_len;

    image->height = ((int *)(buf+height_in_header))[0];
    image->width  = ((int *)(buf+width_in_header))[0];
    image->color_size = ((int16_t *)(buf+color_size_in_header))[0]/BYTE;
    
    ASSERT(image->color_size > 4);

    image->size   = ((int *)(buf+image_size_in_header))[0];

    image->length = (buf_len - HEADER_LEN)/image->color_size;
    //  + HEADER_LEN%COLOR_SIZE));

    image->pixels = (u_int *)calloc(image->length+1, sizeof(u_int));

    memcpy(image->header, buf, HEADER_LEN);

    return image;
};

Img * imageCopyStruct(Img * src, int16_t new_color_size)
{
    Img * image = (Img *)calloc(1, sizeof(Img));
    memcpy(image, src, sizeof(Img));

    if (image->color_size != new_color_size)
    {
        image->color_size = new_color_size;
        image->length_in_chars = HEADER_LEN + image->length*image->color_size;

        ((int16_t *)&image->header[color_size_in_header])[0] = image->color_size*BYTE;
    }

    image->pixels = (u_int *)calloc(image->length+1, sizeof(u_int));

    return image;
};

int imagePrint(Img *image)
{
    printf("Image:\n"
           "\tsize (%dx%d)\n"
           "\tcolor size = %lu\n"
            ,image->width, image->height, image->color_size);

    return 0;
};

int imageDtor(Img *image)
{
    free(image->pixels);
    free(image);
    return 0;
};

#ifdef SSE
        #include <smmintrin.h>
        #include <tmmintrin.h>
        #include <emmintrin.h>
        #include "intrinsics_debug.h"
#endif

#ifndef SSE
Img * alpha_blend(Img *front, Img *back, int x_shift, int y_shift)
{
    x_shift = abs(back->width - front->width - x_shift);
    // y_shift = back->height - front->height - y_shift;

    ASSERT(front->length > back->length);

    int16_t new_color_size = MAX(back->color_size, front->color_size);

    Img * result = imageCopyStruct(back, new_color_size);
    pixel tmp = {};

    for (int yi = 0; yi < result->height; yi++)
    {
        for(int xi = 0; xi < result->width; xi++)
        {
            int delta_x = xi - x_shift;
            int delta_y = yi - y_shift;

            size_t back_counter = yi*back->width + xi;
            unsigned int result_color = 0;

            if ( (0 <= delta_x && delta_x < front->width) && (0 <= delta_y && delta_y < front->height))
            {
               size_t front_counter = (delta_y*front->width + delta_x);
   
                u_int back_pixel = back->pixels[back_counter];
                u_int front_pixel = front->pixels[front_counter];

                u_char front_alpha = front_pixel>>(3*BYTE);
                u_char one_color = 0;
                
                // printf("%2x ", front_alpha);
                // printf("RGB: ( ");
                for(int idx = 0; idx < sizeof(u_int)*BYTE; idx+=BYTE)
                {
                    one_color = (((((0xFF << idx) & front_pixel) >> idx)*front_alpha 
                                    + (((0xFF << idx) & back_pixel) >> idx) * (0xFF - front_alpha)) )/0xFF;
                    // printf("%2x ", one_color);

                    result_color += one_color<<idx; 

                }
                // printf(")\n");

            } else
            {
                result_color = back->pixels[back_counter];
            }

            result->pixels[back_counter] = result_color;

        }
    
    }

    imageDtor(back);
    imageDtor(front);

    return result;

}

#else  
Img * alpha_blend(Img *front, Img *back, int x_shift, int y_shift)
{
    x_shift = abs(back->width - front->width - x_shift);
    // y_shift = back->height - front->height - y_shift;

    ASSERT(front->length > back->length);

    int16_t new_color_size = MAX(back->color_size, front->color_size);

    Img * result = imageCopyStruct(back, new_color_size);
    pixel tmp = {};

    for (int yi = 0; yi < result->height; yi++)
    {
        for(int xi = 0; xi < result->width; xi++)
        {
            int delta_x = xi - x_shift;
            int delta_y = yi - y_shift;

            size_t back_counter = yi*back->width + xi;
            unsigned int result_color = 0;

            if ( (0 <= delta_x && delta_x < front->width) && (0 <= delta_y && delta_y < front->height))
            {
                size_t front_counter = (delta_y*front->width + delta_x);
            const  __m128i _0 = _mm_set1_epi8(0); 

                // printf("%lu ", back_counter);
                __m128i front_pixel =  _mm_load_si128((__m128i const *)(front->pixels[front_counter]));
                __m128i back_pixel  = _mm_load_si128((__m128i const *)(back->pixels[back_counter]));

                __m128i FRONT_PIXEL = (__m128i) _mm_movehl_ps((__m128) _0, (__m128) front_pixel);
                __m128i BACK_PIXEL = (__m128i) _mm_movehl_ps((__m128) _0, (__m128) back_pixel);

                front_pixel = _mm_cvtepi8_epi16 (front_pixel);
                back_pixel = _mm_cvtepi8_epi16 (back_pixel);

                FRONT_PIXEL = _mm_cvtepi8_epi16 (FRONT_PIXEL);                              // сделать воздух более разреженым
                BACK_PIXEL = _mm_cvtepi8_epi16 (BACK_PIXEL);

            const   __m128i moveA = _mm_setr_epi8(15, 14, 15, 14, 15, 14, 15, 14, 6, 5, 6, 5, 6, 5, 6, 5);
            const   __m128i _255  = _mm_setr_epi8(0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255);

                __m128i front_alpha = _mm_shuffle_epi8(front_pixel, moveA);                  // front.a(0,1)
                __m128i FRONT_ALPHA = _mm_shuffle_epi8(FRONT_PIXEL, moveA);                  // front.a(2,3)

                front_pixel = _mm_mul_epu32(front_pixel, front_alpha);
                FRONT_PIXEL = _mm_mul_epu32(FRONT_PIXEL, FRONT_ALPHA);

                front_alpha =  _mm_sub_epi32 (_255, front_alpha);
                FRONT_ALPHA =  _mm_sub_epi32 (_255, FRONT_ALPHA);

                back_pixel = _mm_mul_epu32(back_pixel, front_alpha);
                BACK_PIXEL = _mm_mul_epu32(BACK_PIXEL, FRONT_ALPHA);

                __m128i sum = _mm_add_epi32(front_pixel, back_pixel);
                __m128i SUM = _mm_add_epi32(FRONT_PIXEL, BACK_PIXEL);

            const   __m128i moveSUM = _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 15, 13, 11, 9, 7, 5, 3, 1);

                sum = _mm_shuffle_epi8(sum, moveSUM);
                SUM = _mm_shuffle_epi8(SUM, moveSUM);

                __m128i color = (__m128i)_mm_move_ss((__m128)sum, (__m128)SUM); 

                u_char one_color = 0;
                // printf("%2x ", front_alpha);
                // printf("RGB: ( ");
                for(int counter = 0; counter < sizeof(u_int); counter++)
                {
                    one_color = ((u_char *)&color)[BYTE - 1 - counter];
                    // printf("%2x ", one_color);
                    result_color += one_color<<counter*BYTE; 
                }
                // printf(")\n");
                

            } else
            {
                result_color = back->pixels[back_counter];
            }

            result->pixels[back_counter] = result_color;

        }
    
    }

    imageDtor(back);
    imageDtor(front);

    return result;

}

#endif

int saveAsBMP(Img *result, const char *result_file_name)
{
    int buf_len = result->length_in_chars;

    u_char * buf = (u_char *)calloc(buf_len, sizeof(u_char));

    memcpy(buf, result->header, HEADER_LEN);

    u_char color = 0;
    u_int pixel = 0;
    u_char * dest = ((u_char *)(&buf[HEADER_LEN]));

    size_t counter = result->length - 1;
    for(; counter > 0; counter--)
    {
        pixel = result->pixels[counter];

        // printf("RGB: ( ");
        for(int idx = 0; idx < sizeof(u_int); idx++)
        {
            color = ((0xFF<<idx*BYTE)&pixel)>>idx*BYTE;

            if (idx < result->color_size)
            {
                *(dest+idx) = color; 
                // printf("%2x ", color);
            }
        }
        // printf(")\n");

        dest+=result->color_size;
    }

    imageDtor(result);

    FILE * result_file = fopen(result_file_name, "wb");
    fwrite(buf, sizeof(u_char), buf_len, result_file);
    fclose(result_file);
    free(buf);

    return 0;
}