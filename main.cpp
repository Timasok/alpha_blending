#include <stdio.h>
#include <stdlib.h>

#include "alpha.h"

const int BUF_LEN = 64;
const char * RESULT_IMG_PATH = "img/result.bmp";
const char * BACK_IMG_PATH   = "img/AskhatCat.bmp";
const char * FRONT_IMG_PATH  = "img/Table.bmp";

int main()
{
    Image * back  = getImage(BACK_IMG_PATH);
    Image * front = getImage(FRONT_IMG_PATH);

    imageDtor(back);
    imageDtor(front);

    // alpha_blend(back, front, 0);

    char system_command[BUF_LEN] = {};     
    sprintf(system_command, "xdg-open %s", RESULT_IMG_PATH);
    system(system_command);
    
}
