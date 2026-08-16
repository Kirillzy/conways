#include "functions.h"

// Parses the original file we got in argv[2] into the 2d world array we created
int parse0thGen(short int rows, short int cols, int arr[rows][cols], char *seed) {
    FILE *fpRead = fopen(seed, "r");
    
    if (fpRead == NULL) {
        fprintf(stderr, "ERROR: opening file.\n");
        return 1;
    }

    // This should skip the rows and cols numbers we get
    short int num;
    for (int i = 0; i < 2; i++) {
        // %hd will skip all white space (including \n)
        if (fscanf(fpRead, "%hd", &num) != 1) {
            fprintf(stderr, "ERROR: Expected a number.\n");
            fclose(fpRead);
            return 1;
        }
    }

    // Now we need to correctly write the dead and alive cells into 2d arr
    int ch;

    // Parses the file to get all of the "." and "0" to put into the 2d arr
    for (int i = 0; i < rows; i++) {
        for (int k = 0; k < cols; k++) {

            // Loop until we hit a valid character or EOF
            while ((ch = fgetc(fpRead)) != EOF) {
                if (ch == '.' || ch == '0') {
                    break; // Found what we need!
                }
            }

            if (ch == EOF) {
                fprintf(stderr, "ERROR: Unexpected end of file at [%d][%d]\n", i, k);
                fclose(fpRead);
                return 1;
            }

            // If its a dot, we put '0' to symbolize its a dead cell
            if (ch == '.') {
                arr[i][k] = '0';
            }
            // Else, it has to be '0', so we put '1' to symbolize it is alive
            else {
                arr[i][k] = '1';
            }

        }
    }
    fclose(fpRead);

    for (int i = 0; i < rows; i++) {
        for (int k = 0; k < cols; k++) { 
            printf("%c", arr[i][k]);
        }
        printf("\n");
    }
    fprintf(stdout, "-----------------------------------------------------------------\n");

    return 0;
}

// Gets the file name from argv[2] and appends '_' + the generation number.
char *fileNameGetter(char *seed, short int gen) {
    // file length for when we wanna strncpy etc
    char buffer[7]; // short int max is usually 5-6 digits + null terminator
    snprintf(buffer, sizeof(buffer), "%hd", gen);
    int lengthNum = strlen(buffer);

    // length of seed + underscore (1) + lengthNum;
    long outputFileLength = strlen(seed) + 1 + lengthNum;
    // Making space to put this into the file. +1 (Null terminator)
    char *fileGen = (char*)malloc(sizeof(char)*(outputFileLength + 1));
    // Checking for null
    if (fileGen == NULL) {
        free(seed);
        fprintf(stderr, "Malloc failed for fileGen");
        exit(EXIT_FAILURE);
    }
    
    // This function goes snprintf(destination, buffer_size, "%s%s", string1, string2);
    // Remember, buffer_size stops at buffer_size-1 to leave room for null terminator
    snprintf(fileGen, outputFileLength + 1, "%s_%s", seed, buffer);

    return fileGen;
}

// Uses the 2d array to update the manager by each row, then opens a write file to use manager to write into.
// NOTE: Internally, our world array stores the ASCII chars '0' (dead) and '1' (alive).
// golo_set_row(), however, expects a boolean convention: any nonzero byte counts as alive,
// 0 means dead. This function is the single boundary where that translation happens.
int writeToFile(short int rows, short int cols, int arr[rows][cols], char *fileW, golo_manager_t *m) {
    // Create a temporary array of characters to hold exactly one row
    char temp_row[cols];

    // Loop through each row of the 2D array
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // ASCII character '0' (which is 48)
            // If its dead, set the value to 0.
            if (arr[r][c] == '0') {
                temp_row[c] = 0; // Dead
            } 
            // Else its alive, set value to 1.
            else {
                temp_row[c] = 1; // Alive
            }
        }

        golo_set_row(m, r, temp_row);
    }

    FILE *fpW = fopen(fileW, "w");

    if (fpW == NULL) {
        fprintf(stderr, "ERROR: opening file.\n");
        return 1; // 1 for failure
    }

    // Now we write it into the file
    golo_write_text(m, fpW);

    fclose(fpW);

    return 0;
}

// Writes the current manager state out as a BMP image for the current generation.
// NOTE: This does NOT populate the manager itself - it relies on writeToFile()
// having already been called this generation, since that function is what runs
// golo_set_row() for every row. Always call this AFTER writeToFile() in the loop.
int writeBmpToFile(char *fileW, golo_manager_t *m) {
    // BMP is binary data, so open in binary write mode
    FILE *fpW = fopen(fileW, "wb");
 
    if (fpW == NULL) {
        fprintf(stderr, "ERROR: opening BMP file.\n");
        return 1; // 1 for failure
    }
 
    int ok = golo_write_bmp(m, fpW);
 
    fclose(fpW);
 
    // golo_write_bmp returns 1 on success, 0 on failure
    if (!ok) {
        fprintf(stderr, "ERROR: writing BMP data.\n");
        return 1;
    }
 
    return 0;
}

// This is a helper function for nextGen that ensures nothing is out of bounds.
int _isValid(int r, int c, short int rows, short int cols) {
    return (r >= 0 && r < rows && c >= 0 && c < cols);
}

// Uses Conways Game of Life basic rules to update our 2d array for the next generation.
void nextGen(short int rows, short int cols, int arr[rows][cols], int vert, int horz) {
    // The next world 2d array
    int next[rows][cols];

    /*
    Basic Rules:
    1. Any live cell with fewer than two live neighbors dies, as if by underpopulation
    2. Any live cell with cell with two or three neighbord lives on to the next generation
    3. Any live cell with more than three live cells dies, as if by overpopulation
    4. Any dead cell with exactly three live neighbord becomes a live cell, as if by reproduction
    */

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            
            int neighbors = 0;

            // ---- Check all 8 surrounding cells ----
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    // Skip the cell itself (r, c)
                    if (dr == 0 && dc == 0) continue;

                    int nr = r + dr;
                    int nc = c + dc;

                    // If wrapVert is 1, do wrapping Vertically
                    if (vert == 1) {
                        nr = (r + dr + rows) % rows;
                    }
                    // If wrapVert is 0, dont do wrapping Vertically, so do nothing

                    // If wrapHorz is 1, do wrapping Horizontally
                    if (horz == 1) {
                        nc = (c + dc + cols) % cols;
                    }
                    // If wrapHorz is 0, dont do wrapping Horizontally, so do nothing

                    // ---- Use the helper to check bounds safely ----
                    // This will handle even wrapping since it'll do valid
                    if (_isValid(nr, nc, rows, cols)) {
                        // If the neighbor is alive ('1'), increment count
                        if (arr[nr][nc] == '1') {
                            neighbors++;
                        }
                    }
                }
            }

            // ---- Applying Game of Life rules here ----
            if (arr[r][c] == '1') { // If cell is currently alive
                if (neighbors < 2 || neighbors > 3) {
                    next[r][c] = '0'; // Cell dies
                } else {
                    next[r][c] = '1'; // Cell stays alive
                }
            } 
            else { // If cell is currently dead
                if (neighbors == 3) {
                    next[r][c] = '1'; // Cell becomes alive
                }
                else {
                    next[r][c] = '0'; // Cell dies
                }
            }
        }
    }

    // Now putting next into our current arr to replace.
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            arr[r][c] = next[r][c];
        }
    }
}
