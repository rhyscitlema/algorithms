# Makefile created by Rhyscitlema
# Explanation of file structure available at:
# http://rhyscitlema.com/applications/makefile.html

OUTPUT_FILE = libalgo.a

OBJECT_FILES = tree.o \
               list.o \
               heap.o \
               graph.o \
               fileio.o

#-------------------------------------------------

# C compiler
CC = gcc

# archiver
AR = ar

# compiler flags
CC_FLAGS = -Wall -std=c99 -pedantic $(CFLAGS)

# archiver flags
AR_FLAGS = -crs #$(ARFLAGS)

#-------------------------------------------------

make: $(OUTPUT_FILE)

$(OUTPUT_FILE): $(OBJECT_FILES)
	$(AR) $(AR_FLAGS) $(OUTPUT_FILE) $(OBJECT_FILES)

# remove all created files
clean:
	$(RM) *.o *.a

#-------------------------------------------------

tree.o: tree.c tree.h
	$(CC) $(CC_FLAGS) -c -o $@ $<

list.o: list.c list.h
	$(CC) $(CC_FLAGS) -c -o $@ $<

heap.o: heap.c heap.h
	$(CC) $(CC_FLAGS) -c -o $@ $<

graph.o: graph.c graph.h heap.h
	$(CC) $(CC_FLAGS) -c -o $@ $<

fileio.o: fileio.c fileio.h
	$(CC) $(CC_FLAGS) -c -o $@ $<

