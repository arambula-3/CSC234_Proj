#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Proj_Interpreter_V5.1.c"

// Function to process test data from a file
int fullProgramTest();

int main() {
    printf("Starting file-based black box testing for proj_interpreter...\n");

    int fpt_check = fullProgramTest();
    if (fpt_check == 0) {
        printf("Testing for full program Passed.\n");
    } else {
        printf("Testing for full program Failed.\n");
    }

    printf("File-based black box testing for proj_interpreter completed.\n");
    return 0;
}

int fullProgramTest() {
    FILE *fp2 = fopen("../../test_texts/prog_test1_validate.txt","r");

    if (fp2 == NULL) {
        perror("Error opening test data file");
        return 0;
    }

    char *argv[] = {"../../Proj_Interpreter_V5.1.exe","../../test_texts/nothing.txt"};

    interpreter(2, argv);

    FILE *fp1 = fopen("../../proj_interpreter_test_files/output.txt", "r");
    if (fp1 == NULL) {
        perror("Error opening test data file");
        return 0;
    }

    char line1[BUFFERSIZE];
    char line2[BUFFERSIZE];
    int line_check = 0;
    while (fgets(line1, BUFFERSIZE, fp1) != NULL && fgets(line2, BUFFERSIZE, fp2) != NULL) {
        if (strcmp(line1,line2) == 0) {
            line_check = 1;
        } else {
            line_check = 0;
            break;
        }
    }
    fclose(fp1); // Close the test data file
    fclose(fp2);
    return line_check;
}