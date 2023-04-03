#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "alpha.h"
#include "text_funcs.h"

static int sign_len = 14;

Image * getImage(const char *source_file_name)
{
    Text_info src = {};
    textCtor(&src, source_file_name);

    printText(&src);

//add check signature
    Image * image = imageCtor(src.buf_length);

    // DBG_OUT;
    for (int counter = 0; counter < image->length; counter++)
    {
        image->pixels[counter].r = src.buf[sign_len+counter*sizeof(int) + 0];
        image->pixels[counter].g = src.buf[sign_len+counter*sizeof(int) + 1];
        image->pixels[counter].b = src.buf[sign_len+counter*sizeof(int) + 2];
        image->pixels[counter].a = src.buf[sign_len+counter*sizeof(int) + 3];
    }

    textDtor(&src);

    return image;
}

Image * imageCtor(size_t buf_len)
{
    Image * image = (Image *)calloc(1, sizeof(Image));
    image->length_in_chars = buf_len;
    image->length = (buf_len - (sign_len + 2) )/sizeof(int);
    image->pixels = (pixel *)calloc(image->length, sizeof(pixel));

#ifdef DEBUG
    printf("%d\n", image->length);
    sleep(1);
#endif
    return image;
};

int imagePrint(Image *image)
{
    for (size_t counter = 0; counter < image->length; counter++)
    {
        printf("(%d %d %d %d)\n", 
        image->pixels[counter].r, image->pixels[counter].g, 
        image->pixels[counter].b, image->pixels[counter].a);

    }
    return 0;
};

int imageDtor(Image *image)
{
    free(image->pixels);
    free(image);
    return 0;
};

Image * alpha_blend(Image *front, Image *back, int front_shift)
{
    ASSERT(front->length > back->length);

    Image * result = imageCtor(back->length_in_chars);

    for (size_t counter = 0; counter < result->length; counter++)
    {

        // if (counter >= front_shift && (counter - front_shift) < front->length )
        // {
        //     // result->pixels[counter].r = (front->pixels[counter].r*front->pixels[counter].a + 
        //     //                             back->pixels[counter].r*(255 - front->pixels[counter].a))/255;
        //     // result->pixels[counter].g = (front->pixels[counter].g*front->pixels[counter].a + 
        //     //                             back->pixels[counter].g*(255 - front->pixels[counter].a))/255;
        //     // result->pixels[counter].b = (front->pixels[counter].b*front->pixels[counter].a + 
        //     //                             back->pixels[counter].b*(255 - front->pixels[counter].a))/255;
        //     // result->pixels[counter].a = (front->pixels[counter].a*front->pixels[counter].a + 
        //     //                             back->pixels[counter].a*(255 - front->pixels[counter].a))/255;
        //     result->pixels[counter].r = 100;
        //     result->pixels[counter].g = 100;
        //     result->pixels[counter].b = 100;
        //     result->pixels[counter].a = 100;

        // } else
        // {
            result->pixels[counter].r = back->pixels[counter].r;
            result->pixels[counter].g = back->pixels[counter].g;
            result->pixels[counter].b = back->pixels[counter].b;
            result->pixels[counter].a = back->pixels[counter].a;

        // }


#ifdef DEBUG
    printf("%d\n", counter);
#endif

    }
    // sleep(1);

    imageDtor(back);
    imageDtor(front);

    return result;

}

int saveAsBMP(Image *result, const char *result_file_name)
{
    int buf_len = result->length_in_chars;

    char * buf = (char *)calloc(buf_len, sizeof(char));

    buf[0] = 'B';
    buf[1] = 'M';

    *((int *)(buf + 2)) = buf_len;
    
    for (size_t counter = 0; counter < result->length; counter++)
    {
        buf[sign_len+counter*sizeof(int) + 0] = result->pixels[counter].r;
        buf[sign_len+counter*sizeof(int) + 1] = result->pixels[counter].g;
        buf[sign_len+counter*sizeof(int) + 2] = result->pixels[counter].b;
        buf[sign_len+counter*sizeof(int) + 3] = result->pixels[counter].a;
    }

    imageDtor(result);

    FILE * result_file = fopen(result_file_name, "wb");
    fwrite(buf, sizeof(char), buf_len, result_file);
    fclose(result_file);
    free(buf);

    return 0;
}