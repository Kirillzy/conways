// Header file
#include "gameOfLifeOutput.h"
// My header file
#include "functions.h"

int main(int argc, char *argv[]) {

    // Required argv[1]
    short int gens;

    // Required argv[2] seed 
    char *seed;
    short int rows = 0;
    short int cols = 0;

    // Optional argv[3]
    int wrapVert = 0;
    // Optional argv[4]
    int wrapHorz = 0;
    
    // Command Arguements Handling where we handles arguements 1-4

    /*
    // Check if size (argc) is equal or greater than 3 {
        
        // --- First Arg ---
        // Check for if the first arg (argv[1]) is a non-negative integer or missing
            // Error out
        // Else output the seed configuration (0) + requested number of gens
            // If 0, just seed configuration
        
        // --- Second Arg ---
        // Check if the second arg (argv[2]) is missing or if the file is missing
            // Error out
        // Else read from that file since our start seed config
    // }
    */
    fprintf(stdout, "-----------------------------------------------------------------\n");

    if (argc >= 3) {
        // --- First Arg ---
        char *endptr;
        gens = strtol(argv[1], &endptr, 10);

        // Basic check: if endptr still points to the start, no digits were found
        if (argv[1] == endptr) {
            fprintf(stderr, "ERROR: No digits were found, so they are missing in the first arguement.\n");
            exit(EXIT_FAILURE);
        }

        // Checking if the first arg is a negative integer to handle error
        if (gens < 0) {
            fprintf(stderr, "ERROR: The first arguements digits are negative, which is not permitted.\n");
            exit(EXIT_FAILURE);  
        }

        // --- Second Arg ---
            // Parse through the Seed File Format correctly, 
            // First line has the 2 integers in the file
            // if it exists, check if . and 0 are equal to rows*cols
        
        // I get the file given, then put it into our file scope
        char *fileGivenString = argv[2];
        // mallocing here to string copy correctly into it plus null terminator
        seed = (char *)malloc(sizeof(char)*(strlen(fileGivenString) + 1));

        if (seed == NULL) {
            fprintf(stderr, "ERROR: Malloc failed for seed.\n");
            exit(EXIT_FAILURE);
        }
        // Should be safer since we have the exact size
        strcpy(seed, fileGivenString);

        // Opening the file to parse through to get rows and cols
        // as well as checking for incomplete . and 0 [(all . and 0) = rows*cols]
        FILE *fpRead = fopen(seed, "r");

        if (fpRead == NULL) {
            free(seed);
            fprintf(stderr, "ERROR: fopen has failed for fpRead.\n");
            exit(EXIT_FAILURE);
        }

        /*
        3.2 Seed File Format
        ---- 2 integers by whitespace, rows and cols (respectively)
        ---- Total num of '.' and '0' >= rows*cols, error otherwise 
        */
       
        // First the rows and cols
        for (int i = 0; i < 2; i++) {

            short int num;
            int check = fscanf(fpRead, "%hd", &num);
            if (check != 1) {
                // If we make it here, that means there's an error
                free(seed);
                fclose(fpRead);
                fprintf(stderr, "ERROR: Expected a number, but found something else.\n");
                exit(EXIT_FAILURE);
            } 
            
            // Check if the number is positive (handles 0 or negative)
            if (num <= 0) {
                free(seed);
                fclose(fpRead);
                fprintf(stderr, "ERROR: Dimensions must be positive integers.\n");
                exit(EXIT_FAILURE);
            }

            // Row
            if (i == 0) {
                rows = num;
            } 
            // Col
            else {
                cols = num;
            }
        }

        fprintf(stdout, "rows: %hd cols: %hd\n", rows, cols);

        int ch;
        // counts '.' and '0' to make sure >= rows*cols
        int count = 0;
        while ((ch = fgetc(fpRead)) != EOF) {

            if (ch == '.' || ch == '0') {
                count++;
            } 
        }

        // Now check if count >= rows*cols by checking count < rows*cols for error
        if (count < (rows*cols)) {
            free(seed);
            fclose(fpRead);
            fprintf(stderr, "ERROR: Total number of periods and zeros in the file is LESS than rows*cols.\n");
            exit(EXIT_FAILURE);
        }

        // Close this since we confirmed our seed configuration file is error free
        fclose(fpRead);

    } 
    // Else we dont have the 2 required arguements, error out
    else {
        fprintf(stderr, "ERROR: We are missing arg 1 and/or arg 2, cannot proceed.\n");
        exit(EXIT_FAILURE);
    }

    /*
    --- Optional Arg Checks ---
    Arg 3 and 4 are defaulted to being "0"

    Check if size (argc) is equal or greater than 4 {
        If the arg 3 (argv[3]) is "1", make the wrapVertically boolean true (1)
        Else if "0", we basically just confirm that yes, wrapVert is 0 to false already. (0)
        Else, we error out since if argv[3] is provided, we need it to be "1" or "0"

        If argc == 5, then we check whats inside {
            If arg 4 (argv[4]) is "1", make the wrapHorizontally boolean true (1)
            Else if "0", we basically just confirm that yes, wrapHorz is 0 to false already. (0)
            Else, we error out since if argv[4] is provided, we need it to be "1" or "0" }
    }
    */

    if (argc >= 4) {
        // --- Third Arg --- 
        if ((strcmp(argv[3], "0") == 0)) {
            wrapVert = 0;
            fprintf(stdout, "wrapVert is now 0\n");
        }
        else if (strcmp(argv[3], "1") == 0) {
            wrapVert = 1;
            fprintf(stdout, "wrapVert is now 1\n");
        }
        else {
            free(seed);
            fprintf(stderr, "ERROR: arg 3 was provided, but it was not a 1 or 0.\n");
            exit(EXIT_FAILURE);
        }

        // --- Fourth Arg ---
        if (argc == 5) {

            if (strcmp(argv[4], "0") == 0) {
                wrapHorz = 0;
                fprintf(stdout, "wrapHorz is now 0\n");
            }
            else if (strcmp(argv[4], "1") == 0) {
                wrapHorz = 1;
                fprintf(stdout, "wrapHorz is now 1\n");
            }
            else {
                free(seed);
                fprintf(stderr, "ERROR: arg 4 was provided, but it was not a 1 or 0.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    fprintf(stdout, "-----------------------------------------------------------------\n");

    // Next I will store my world with a 2d array
    int world[rows][cols];

    // Parses the original file we read into for the 2d arr world to store.
    int parse = parse0thGen(rows, cols, world, seed);

    // If parse is 1, then we have a failure, so we quit with free's and failure (1)
    if (parse == 1) {
        free(seed);
        exit(EXIT_FAILURE);
    }

    // Initialize the manager
    golo_manager_t *manager = golo_init(rows, cols);
    if (manager == NULL) {
        fprintf(stderr, "ERROR: Failed to initialize golo_manager.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < gens + 1; i++) {
        
        // Here we would get the correct file name for generation
        char *gensFile = fileNameGetter(seed, i);
        // Testing for if the fileNameGetter got it correct
        fprintf(stdout, "%s\n", gensFile);

        // We would write to file here
        printf("write....\n");
        int toFile = writeToFile(rows, cols, world, gensFile, manager);

        // if writeToFile failed, we free everything and exit.
        if (toFile == 1) {
            free(seed);
            golo_free(manager);
            exit(EXIT_FAILURE);
        }

        // This is because it would generate a next below that we dont need
        if (gens == i) {
            free(gensFile);
            break;
        }

        nextGen(rows, cols, world, wrapVert, wrapHorz);

        free(gensFile);

    }
    fprintf(stdout, "-----------------------------------------------------------------\n");

    // This is where we free what needs to be free'd
    golo_free(manager);
    free(seed);
    
    return 0;
}
