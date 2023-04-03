#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include "text_funcs.h"

const char* TXT_BORDER = "************************************************************";

static FILE *text_logs;
static int vacant_log_num = 0;

#define ASSERT_OK(textPtr)                                                  \
   do{                                                                      \
      if (returnTextError(textPtr) != 0)                                    \
       {  /*fprintf(stderr, "DUMP_CALLED: file: %s func: %s line: %d\n",*/  \
                            /*  __FILE__, __FUNCTION__, __LINE__); */       \
           printText(textPtr);                                              \
       }                                                                    \
   } while (0)

#define PRINT_LOG(...)                                                  \
        do {                                                            \
            fprintf(text_logs, __VA_ARGS__);                            \
        } while(0)   

int openTextLogs()
{
    text_logs = fopen("./logs/text_log.txt", "w+");
    return 0;
}

int closeTextLogs()
{
    fclose(text_logs);
    return 0;
}

int textCtor(Text_info * text, const char * file_name)
{
    ASSERT((file_name != nullptr));
    openTextLogs();

    text->file_name = strdup(file_name);
    text->source_file = fopen(file_name, "rb");
    text->log_num = vacant_log_num;
    vacant_log_num++;
    
    ASSERT((text->source_file != nullptr));

    struct stat data = {};
    stat(file_name, &data);
    text->buf_length = data.st_size;

    // fseek(text->source_file, 0L, SEEK_END);
    // text->buf_length = ftell(text->source_file);
    // char * buf = (char *)calloc(data.st_size + 2, sizeof(char));
    // free(buf);

    text->buf = (char *)calloc(text->buf_length + 1, sizeof(char));

    fread(text->buf, sizeof(char), text->buf_length, text->source_file);

    fclose(text->source_file);

    return 0;
}

int textDtor(Text_info * text)
{
    text->source_file = fopen(text->file_name, "wb");
    fwrite(text->buf, sizeof(char), text->buf_length, text->source_file);
    fclose(text->source_file);

    free(text->file_name);
    free(text->buf);
    closeTextLogs();
    return 0;
}

int printText(Text_info * text)
{
    ASSERT((text != nullptr));

    closeTextLogs();

    const int BUF_LEN = 64;
    char file_name[BUF_LEN] = {};     
    sprintf(file_name, "./logs/text_log_%d.txt", text->log_num);

    text_logs = fopen(file_name, "w+");

    PRINT_LOG("\n%s\n", TXT_BORDER);
    PRINT_LOG("FILE NAME = %s\n", text->file_name);
    PRINT_LOG("FILE = %p\n", text);
    PRINT_LOG("Code_of_error = %d\n", text->code_of_error);
    PRINT_LOG("Buf_length = %ld\n", text->buf_length);
    PRINT_LOG("\n%s\n", TXT_BORDER);

    // for(int i = 0; i < text->buf_length; i++)
    //     PRINT_LOG("%2x ", text->buf[i]);

    closeTextLogs();
    text_logs = fopen("./logs/text_log.txt", "a+");

    return 0;
}

int stringDump(const char *string, const char * name_of_var, const char * name_of_file, const char * name_of_func, int number_of_line)
{
    printf("\e[0;32m\n%s\e[0m at %s at %s(%d)\n",  name_of_var, name_of_func,
           name_of_file, number_of_line);

    printf("string = %s, string_length = %ld\n", string, strlen(string));

    return 0;
}

int returnTextError(Text_info * text)
{

    text->code_of_error |= ((!text->source_file) * TEXT_ERROR_SOURCE_FILE_IS_NULL);

    text->code_of_error |= ((!text->lines) * TEXT_ERROR_LINES_IS_NULL);

    text->code_of_error |= ((text->number_of_lines < 1) * TEXT_ERROR_NUMBER_OF_LINES_IS_INAPROPRIATE);

    text->code_of_error |= ((!text->buf) * TEXT_ERROR_BUF_IS_NULL);

    return text->code_of_error;

}

// =====^^actual funcs^^=====

#undef ASSERT_OK