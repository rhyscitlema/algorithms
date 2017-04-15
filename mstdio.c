/*
    mstdio.c
*/
#ifdef MSTDIO

#include "mstdio.h"
#include <malloc.h>

void wait() { printf(" press enter..."); getchar(); }


bool string_to_file (const char* filename, const char* filecontent, long contentSize)
{
    FILE* file = fopen(filename, "wb");
    if(file==NULL) return 0;
    fwrite(filecontent, 1, contentSize, file);
    fclose(file);
    return 1;
}


bool file_to_string (const char* filename, char* *filecontent_ptr, long *contentSize_ptr)
{
    long size;
    char* filecontent;

    FILE* file = fopen (filename, "rb");
    if(file==NULL) return 0;

    fseek (file, 0, SEEK_END);
    size = ftell (file);
    fseek (file, 0, SEEK_SET);
    if(size==0x7FFFFFFF) { fclose(file); return 0; }

    filecontent = (char*) malloc (size+1);
    fread (filecontent, 1, size, file);

    fclose(file);
    filecontent[size]=0;

    if(contentSize_ptr!=NULL) *contentSize_ptr = size;
    if(filecontent_ptr!=NULL) *filecontent_ptr = filecontent; else free (filecontent);
    return 1;
}

#endif

