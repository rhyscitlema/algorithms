/*
    fileio.h
*/

typedef struct { char* data; long size; } Array;

/* if first time call then set argument old = {0} */
Array file_to_array (const char* filename, Array old);

