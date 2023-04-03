#include <stdio.h>

#ifndef TEXT_FUNCS_H
#define TEXT_FUNCS_H

#include "general_debug.h"

struct  Text_info
{
    FILE * source_file;
    char *buf;
    size_t buf_length;      //(buf_length in chars)/4
    size_t corrected_adr;
    char * file_name;
    int log_num = 0;

    int code_of_error;
    int number_of_lines;
    const char ** lines;

};

enum TEXT_ERRORS
{
    TEXT_ERROR_SOURCE_FILE_IS_NULL              = 1 << 0,
    TEXT_ERROR_BUF_IS_NULL                      = 1 << 1, 
    TEXT_ERROR_LINES_IS_NULL                    = 1 << 2,
    TEXT_ERROR_NUMBER_OF_LINES_IS_INAPROPRIATE  = 1 << 3, 
};

int textCtor(Text_info * text, const char * file_name);

int openTextLogs();
int closeTextLogs();
int textEdit(Text_info * text);

int textDtor(Text_info * text);
int printText(Text_info * text);


#define STRING_DUMP(line) \
            stringDump(line, #line, __FILE__, __PRETTY_FUNCTION__, __LINE__)


int stringDump(const char *string, const char * name_of_var, const char * name_of_file, const char * name_of_func, int number_of_line);


#endif