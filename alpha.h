struct Image
{
    int length = 0;
    pixel *pixels = 0;
};

struct pixel
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

};

Image * getImage(const char *source_file_name);
int imageDtor(Image *image);
Image * alpha_blend(Image *front, Image *back, int front_shift); // frees-> front, back, allocates bytes for result
int saveAsBMP(const char *result_file_name);                     // frees->result