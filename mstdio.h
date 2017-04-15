#ifndef _MSTDIO_H
#define _MSTDIO_H

#include <stdio.h>
#include <stdbool.h>

extern void wait();

extern bool string_to_file (const char* filename, const char* filecontent, long contentSize);

extern bool file_to_string (const char* filename, char* *filecontent_ptr, long *contentSize_ptr);

#endif

