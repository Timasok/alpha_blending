#include <stdio.h>
#include <stdlib.h>

#include "alpha.h"

const int BUF_LEN = 64;
const char * RESULT_IMG_PATH = "img/result.bmp";
const char * BACK_IMG_PATH   = "img/Table.bmp";
const char * FRONT_IMG_PATH  = "img/AskhatCat.bmp";

int main()
{
    Img * back  = getImage(BACK_IMG_PATH);
    Img * front = getImage(FRONT_IMG_PATH);

    Img * result = alpha_blend(front, back, 100, 100);
    saveAsBMP(result, RESULT_IMG_PATH);

    char system_command[BUF_LEN] = {};     
    sprintf(system_command, "xdg-open %s", RESULT_IMG_PATH);
    system(system_command);
    
}   