#include "my_debug.h"
#include <string.h>

// @ function: 
// @ description:��MDK __FILE__���н�����ֻҪ���һ�е��ļ���
// @ input:
// @ note:Ĭ�ϵ�__FILE__Ϊ���·��
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
