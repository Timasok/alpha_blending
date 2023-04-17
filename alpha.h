#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
// #include <SFML/Clock.hpp>

using namespace sf;

#define SSE
// #define DEBUG

const int BYTE       = 8;
// const int HEADER_LEN = 56;
const int HEADER_LEN = 54;
const int PACK    = 4;

struct pixel
{
    u_char r;
    u_char g;
    u_char b;
    u_char a;

};

struct Img
{
    u_char header[HEADER_LEN] = {};
    size_t length_in_chars = 0;
    size_t length = 0;
    long size = 0;
    int width  = 0;
    int height = 0;
    int16_t color_size = 0;

    u_int *pixels;
};

#define IMG_PRINT(image)                                                                \
    printf("Image:\n"                                                                   \
           "\tvar name: %s"                                                             \
           "\tsize: (%dx%d)\n"                                                          \
           "\tpixels: %d"                                                               \
           "\tbytes_in_one: %lu\n\n"                                                    \
            ,#image, image->width, image->height, image->length, image->color_size)     \


Img * getImage(const char *source_file_name);
Img * imageCtor(size_t buf_len, const char * buf);
int imagePrint(Img *image);
Img * imageCopyStruct(Img * src, int16_t new_color_size);

int imageDtor(Img *image);
Img * alpha_blend(Img *front, Img *back, int x_shift, int y_shift); // frees-> front, back, allocates bytes for result
int saveAsBMP(Img *result, const char *result_file_name);                     // frees->result