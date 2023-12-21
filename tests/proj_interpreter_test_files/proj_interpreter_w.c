#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Proj_Interpreter_V5.1.c"

// Function to process test data from a file
int fullProgramTest1();
int fullProgramTest2();
int fullProgramTest3();
int fullProgramTest4();
int fullProgramTestFaulty();

int main() {
    printf("Starting file-based white box testing for proj_interpreter...\n");

    int fpt1_check = fullProgramTest1();
    int fpt2_check = fullProgramTest2();
    int fpt3_check = fullProgramTest3();
    int fpt4_check = fullProgramTest4(); 
    int fptf_check = fullProgramTestFaulty();
    if (fpt1_check == 1) {
        printf("Testing for full program 1 Passed.\n");
    } else {
        printf("Testing for full program 1 Failed.\n");
    }
    if (fpt2_check == 1) {
        printf("Testing for full program 2 Passed.\n");
    } else {
        printf("Testing for full program 2 Failed.\n");
    }
    if (fpt3_check == 1) {
        printf("Testing for full program 3 Passed.\n");
    } else {
        printf("Testing for full program 3 Failed.\n");
    }
    if (fpt4_check == 1) {
        printf("Testing for full program 4 Passed.\n");
    } else {
        printf("Testing for full program 4 Failed.\n");
    }
    if (fptf_check == 1) {
        printf("Testing for full program faulty Passed.\n");
    } else {
        printf("Testing for full program faulty Failed.\n");
    }

    printf("File-based white box testing for proj_interpreter completed.\n");
    return 0;
}

int fullProgramTest1() {
    FILE *fp2 = fopen("../../test_texts/prog_test1_validate.txt","r");

    if (fp2 == NULL) {
        perror("Error opening test data file");
        return 0;
    }

    char *argv[] = {"../../Proj_Interpreter_V5.1.exe","../../test_texts/prog_test1.txt"};

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

int fullProgramTest2() {
    FILE *fp2 = fopen("../../test_texts/prog_test2_validate.txt","r");

    if (fp2 == NULL) {
        perror("Error opening test data file");
        return 0;
    }

    char *argv[] = {"../../Proj_Interpreter_V5.1.exe","../../test_texts/prog_test2.txt"};

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

int fullProgramTest3() {
    FILE *fp2 = fopen("../../test_texts/prog_test3_validate.txt","r");

    if (fp2 == NULL) {
        perror("Error opening test data file");
        return 0;
    }

    char *argv[] = {"../../Proj_Interpreter_V5.1.exe","../../test_texts/prog_test3.txt"};

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

int fullProgramTest4() {
    FILE *fp2 = fopen("../../test_texts/prog_test4_validate.txt","r");

    if (fp2 == NULL) {
        perror("Error opening test data file");
        return 0;
    }

    char *argv[] = {"../../Proj_Interpreter_V5.1.exe","../../test_texts/prog_test4.txt"};

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

int fullProgramTestFaulty() {
    FILE *fp2 = fopen("../../test_texts/prog_test_faulty_validate.txt","r");

    if (fp2 == NULL) {
        perror("Error opening test data file");
        return 0;
    }

    char *argv[] = {"../../Proj_Interpreter_V5.1.exe","../../test_texts/prog_test_faulty.txt"};

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