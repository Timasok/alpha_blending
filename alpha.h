// #define SSE
// #define DEBUG

const int HEADER_LEN = 58;
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
    int width  = 0;
    int height = 0;
    pixel *pixels;
};

Img * getImage(const char *source_file_name);
Img * imageCtor(size_t buf_len, const char * buf);
int imagePrint(Img *image);

int imageDtor(Img *image);
Img * alpha_blend(Img *front, Img *back, int x_shift, int y_shift); // frees-> front, back, allocates bytes for result
int saveAsBMP(Img *result, const char *result_file_name);                     // frees->result