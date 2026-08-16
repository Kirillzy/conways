#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdlib.h> // needed for malloc and NULL
#include <stdio.h> // needed for fprintf, File I/O, etc... [Already provided in gameOfLifeOutput.h]
#include <ctype.h> // For isalpha()
#include <string.h> // For strlen, strncpy, etc...
#include "gameOfLifeOutput.h"

// This should parse the file we were given in argv[2] correctly
/* 
Pass in rows, cols, the 2d array for said row/cols, 
and the file name to read into that was gotten from argv[2].

Then with all of that information, this function will open the file name given.
With that file opened, it will parse and set '.' as '0' for dead cells, and
'0' as '1' for alive cells inside the 2d array.

Returns 1 for failure, 0 for success.
*/
int parse0thGen(short int rows, short int cols, int arr[rows][cols], char *seed);

// This function gets the file name argv[2] and appends "_g", g being the generation number
// We will pass in the original seed file from argv[2], and generation we are on
// seed - the original argv[2] file we read from, in this case use it for appending
// i - the generation iteration we are on currently
/*
Pass in a char array (seed) and the generation number.

From there, we return a char * that contains ("seed" + "_"+ "gen" + "\0").
Ensures that what we return will not cause a buffer overflow.
*/
char *fileNameGetter(char *seed, short int gen);

// Writes the World into the file for readability purposes
// -- rows, cols, arr is the 2d world array that we will use to write on screen
// -- the fileW is the file we will open to write into.
// -- m is the manager
// Its job is to first, use golo_set_row() to set everything up, then we would write that manager to file
/*
Pass in a 2d array, char *file to create for writing, and the golo_manager_t *m to handle updating rows and to write into file.

First, it parses the 2d arr row by row to update golo_manager_t *m using golo_set_row().
After all the rows have been accounted for, fopen the fileW in write mode.
In that file in write mode, uses golo_write_text(m, fpW) to output the file.
Lastly, handles closing the file with fclose.

Returns 1 for any errors or failures, 0 for success.
*/
int writeToFile(short int rows, short int cols, int arr[rows][cols], char *fileW, golo_manager_t *m);

// Writes the current world state out as a BMP image, for visualization.
// -- fileW is the file to create for writing, in binary mode
// -- m is the manager, which must ALREADY be populated for the current generation
/*
Pass in a char *file to create for writing, and the golo_manager_t *m.
 
IMPORTANT: this function does not call golo_set_row() itself - it assumes the
manager already holds the current generation's data. In practice, this means it
must always be called AFTER writeToFile() within the same generation's iteration,
since writeToFile() is what populates the manager row by row.
 
Opens fileW in binary write mode, calls golo_write_bmp(m, fpW) to render the image,
then closes the file.
 
Returns 1 for any errors or failures, 0 for success.
*/
int writeBmpToFile(char *fileW, golo_manager_t *m);

// Goes into the next generation of our world
// I will put my 2d array here, the function will create another 2d array with same rows/cols.
// Then it will use the array I passed in as a "read" tool and the new array called next as the new one
/*
Pass in a 2d arr, and if there needs to be vertical or horizontal wrapping (0 - No, 1 - Yes).

Creates a new 2d array with the same rows and cols as what was passed in.
Uses the current arr passed in to apply Conways Game of Life basic rules onto the next 2d arr, which accounts for any wrapping needed and out of bounds checks.
After the next 2d arr is finished, it is looped through to apply it to the current array to set up for the next generation.
*/
void nextGen(short int rows, short int cols, int arr[rows][cols], int vert, int horz);



#endif
