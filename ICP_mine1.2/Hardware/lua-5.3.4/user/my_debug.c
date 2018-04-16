#include "my_debug.h"
#include <string.h>

// @ function: 
// @ description:对MDK __FILE__进行解析，只要最后一行的文件名
// @ input:
// @ note:默认的__FILE__为相对路径
const char* clean_filename(const char* path)
{
    const char* filename = path + strlen(path); 
    while(filename > path)
    {
        if(*filename == '/' || *filename == '\\')
        {
            return filename + 1;
        }
        filename--;
    }
    return path;
}
