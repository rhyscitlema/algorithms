/*
    fileio.c
*/

#include "fileio.h"
#include <stdio.h>
#include <malloc.h>


Array file_to_array (const char* filename, Array old)
{
    Array out={0};

    FILE* file = fopen (filename, "rb");
    if(file==NULL) out.size = -1;
    else
    {
        fseek (file, 0, SEEK_END);
        out.size = ftell (file);
        fseek (file, 0, SEEK_SET);

        if(out.size == 0x7FFFFFFF) out.size = -2;
        else if(old.size != -1)
        {
            out.data = (char*) realloc (old.data, out.size+1);

            if(fread(out.data, 1, out.size, file) != out.size)
            {
                free(out.data);
                out.data=NULL;
            }
            else out.data[out.size]=0;
        }
        fclose(file);
    }
    return out;
}

