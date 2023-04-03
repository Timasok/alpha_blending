struct pixel
{
    char r;
    char g;
    char b;
    char a;

};

struct Image
{
    size_t length_in_chars = 0;
    size_t length = 0;
    pixel *pixels;
};

Image * getImage(const char *source_file_name);
Image * imageCtor(size_t buf_len);
int imagePrint(Image *image);

int imageDtor(Image *image);
Image * alpha_blend(Image *front, Image *back, int front_shift); // frees-> front, back, allocates bytes for result
int saveAsBMP(Image *result, const char *result_file_name);                     // frees->result