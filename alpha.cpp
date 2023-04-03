#include <stdio.h>
#include <stdlib.h>

#include "alpha.h"
#include "text_funcs.h"

Image * getImage(const char *source_file_name)
{
    Text_info src = {};
    textCtor(&src, source_file_name);

    printText(&src);

//add check signature
    Image * img = (Image *)calloc(1, sizeof(Image));
    img->length = (src.buf_length - 2)/4;

    for (int counter = 0; counter < img->length; counter++)
    {
        img->pixels[counter].r = src.buf[2+counter*sizeof(int) + 0];
        img->pixels[counter].g = src.buf[2+counter*sizeof(int) + 1];
        img->pixels[counter].b = src.buf[2+counter*sizeof(int) + 2];
        img->pixels[counter].a = src.buf[2+counter*sizeof(int) + 3];
    }

    textDtor(&src);
}

Image * alpha_blend(Image *front, Image *back, int front_shift)
{
    Image *result = (Image *)calloc(1, sizeof(Image));

    ASSERT(front->length > back->length);

}

int imageDtor(Image *image)
{
    free(image->pixels);
    free(image);
};

int saveAsBMP(Image *result, const char *result_file_name);