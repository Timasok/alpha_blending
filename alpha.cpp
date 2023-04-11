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
        ((unsigned char *)dest)[idx] = ((unsigned char *)src)[idx];   
    }

    return 0;
}

Img * getImage(const char *source_file_name)
{
    Text_info src = {};
    textCtor(&src, source_file_name);

    printText(&src);
    
    Img * image = imageCtor(src.buf_length, src.buf);

    // DBG_OUT;
    for (int counter = 0; counter < image->length; counter++)
    {
        setPixel(image->pixels + counter, src.buf + HEADER_LEN + counter*sizeof(unsigned int));
    }

    textDtor(&src);

    return image;
}

Img * imageCtor(size_t buf_len, const char * buf)
{
    const int width_in_header = 18;
    const int height_in_header = 22;
    const int image_size_in_header  = 32;
    const int x_pixels_per_meter_in_header = 38;
    const int y_pixels_per_meter_in_header = 42;

    Img * image = (Img *)calloc(1, sizeof(Img));
    image->length_in_chars = buf_len;

    image->pixels = (unsigned int *)calloc(image->length+1, sizeof(unsigned int));

    image->height = ((int *)(buf+height_in_header))[0];
    image->width  = ((int *)(buf+width_in_header))[0];
    image->size   = ((int *)(buf+image_size_in_header))[0];

    // image->x_pixels_per_meter = ((int *)(buf+x_pixels_per_meter_in_header))[0];
    // image->y_pixels_per_meter = ((int *)(buf+y_pixels_per_meter_in_header))[0];

    /*
    image->x_pixels_per_meter = 1;
    image->y_pixels_per_meter = 1;

    image->width*=image->x_pixels_per_meter;
    image->height*=image->y_pixels_per_meter;
    */

    image->length = (buf_len - (HEADER_LEN + HEADER_LEN%4))/sizeof(int);
    // image->length = image->width*image->height;

    printf("%lu\n", image->length);
    // printf("%u\n", image->size);
    // printf("per meter %d %d\n", image->x_pixels_per_meter, image->y_pixels_per_meter);
    // sleep(3);

    memcpy(image->header, buf, HEADER_LEN);

    return image;
};

Img * imageCopyStruct(Img * src)
{
    Img * image = (Img *)calloc(1, sizeof(Img));
    memcpy(image, src, sizeof(Img));

    // image->pixels = (pixel *)calloc(image->length+1, sizeof(pixel));
    image->pixels = (unsigned int *)calloc(image->length+1, sizeof(unsigned int));

    return image;
};

int imagePrint(Img *image)
{
    for (size_t counter = 0; counter < image->length; counter++)
    {
        // printf("(%d %d %d %d)\n", 
        // image->pixels[counter].r, image->pixels[counter].g, 
        // image->pixels[counter].b, image->pixels[counter].a);

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

    // printf("back_length = %lu\n", back->length);    
    // printf("back:  (%lu, %lu)\n", back->width, back->height);    
    // printf("front: (%lu, %lu)\n", front->width, front->height);
    // sleep(3);

    for (int yi = 0; yi < result->height; yi++)
    {
        for(int xi = 0; xi < result->width; xi++)
        {
            int delta_x = xi - x_shift;
            int delta_y = yi - y_shift;

            size_t back_counter = yi*back->width + xi;
            unsigned int result_color = 0;

            // if ( (0 <= delta_x && delta_x < front->width) && (0 <= delta_y && delta_y < front->height))
            // {
            //     size_t front_counter = (delta_y*front->width + delta_x);

            //     unsigned int back_pixel = back->pixels[back_counter];
            //     unsigned int front_pixel = back->pixels[front_counter];

            //     unsigned char back_alpha = back_pixel>>24;
            //     unsigned char front_alpha = front_pixel>>24;

            //     unsigned char one_color = 0;

            //     for (int idx = 0; idx < sizeof(unsigned int)*8; idx+=sizeof(unsigned char)*8)
            //     {
            //         one_color = ((((0xFF << idx) & front_pixel) >> idx)*front_alpha 
            //                         + (((0xFF << idx) & back_pixel) >> idx) * (0xFF - front_alpha))/0xFF;
            //         result_color += one_color<<idx; 
            //     }

            // } else
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
    
    for (size_t counter = 0; counter < result->length; counter++)
    {
        setPixel(buf+HEADER_LEN+counter*sizeof(int), result->pixels+counter);
    }

    imageDtor(result);

    FILE * result_file = fopen(result_file_name, "wb");
    fwrite(buf, sizeof(unsigned char), buf_len, result_file);
    fclose(result_file);
    free(buf);

    return 0;
}