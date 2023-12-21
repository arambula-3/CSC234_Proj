#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../mcode_parse.c"  // Include the mcode_parse function

// Test function declarations
int testMcodeParse_M03_4();
int testMcodeParse_M06_16();

int main() {
    printf("Starting white box testing for mcode_parse...\n");

    int num_m03_4_pass = testMcodeParse_M03_4();
    int num_m06_16_pass = testMcodeParse_M06_16();
    printf("%d of 9 M3/4 cases Passed\n", num_m03_4_pass);
    printf("%d of 9 M6/16 cases Passed\n", num_m06_16_pass);

    printf("White box testing for mcode_parse completed.\n");
    return 0;
}

int testMcodeParse_M03_4() {
    char *case1[] = {"S100"};
    char *case2[] = {"S"}; //error
    char *case3[] = {"S5", "X25"}; //error
    char *case4[] = {"S5", "T25"}; //error
    char *case5[] = {"T"}; //error
    char *case6[] = {"X"}; //error
    char *case7[] = {"Sh"}; //error
    char *case8[] = {""}; //error
    char *case9[] = {" "}; //error
    char *unit_tests[] = {case1, case2, case3, case4, case5, case6, case7, case8, case9};

    int *case_len[] = {1,1,2,2,1,1,1,1,1};

    char *mcode = "M03";
    char curr_tool[10] = "n/a";
    char spindle_speed[10] = "n/a";
    int track = 0;
    int num_passed = 0;

    while (track < 9) {
        strcpy(curr_tool,"n/a");
        strcpy(spindle_speed,"n/a"); 

        int result = mcode_parse(mcode, unit_tests[track], case_len[track], curr_tool, spindle_speed);
        if ((track < 1 && result == 1) || (track >= 1 && result == 0)) {
            num_passed = num_passed + 1;
            printf("testMcodeParse_M03_4: case %d Passed\n", track + 1);
        } else {
            printf("testMcodeParse_M03_4: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }
    return num_passed;
}

int testMcodeParse_M06_16() {
    char *case1[] = {"T5"};
    char *case2[] = {"T"}; //error
    char *case3[] = {"T5", "X25"}; //error
    char *case4[] = {"T5", "S25"}; //error
    char *case5[] = {"S"}; //error
    char *case6[] = {"X"}; //error
    char *case7[] = {"Th"}; //error
    char *case8[] = {""}; //error
    char *case9[] = {" "}; //error
    char *unit_tests[] = {case1, case2, case3, case4, case5, case6, case7, case8, case9};

    int *case_len[] = {1,1,2,2,1,1,1,1,1};

    char *mcode = "M06";
    char curr_tool[10] = "n/a";
    char spindle_speed[10] = "n/a";
    int track = 0;
    int num_passed = 0;

    while (track < 9) {
        strcpy(curr_tool,"n/a");
        strcpy(spindle_speed,"n/a"); 

        int result = mcode_parse(mcode, unit_tests[track], case_len[track], curr_tool, spindle_speed);
        if ((track < 1 && result == 1) || (track >= 1 && result == 0)) {
            num_passed = num_passed + 1;
            printf("testMcodeParse_M06_16: case %d Passed\n", track + 1);
        } else {
            printf("testMcodeParse_M06_16: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }
    return num_passed;
}
