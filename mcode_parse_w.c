#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcode_parse.c"  // Include the mcode_parse function

// Test function declarations
void testMcodeParse_M03();
void testMcodeParse_M04();
void testMcodeParse_M06();
void testMcodeParse_M16();

int main() {
    printf("Starting white box testing for mcode_parse...\n");

    testMcodeParse_M03();
    testMcodeParse_M04();
    testMcodeParse_M06();
    testMcodeParse_M16();

    printf("White box testing for mcode_parse completed.\n");
    return 0;
}

void testMcodeParse_M03() {
    char *mcode = "M03";
    char *args[] = {"S1500"};
    int len = 1;
    char curr_tool[10] = "n/a";
    char spindle_speed[10];

    int result = mcode_parse(mcode, args, len, curr_tool, spindle_speed);

    // Validate the behavior for M03 command
    if (result == 1 && strcmp(spindle_speed, "1500") == 0) {
        printf("testMcodeParse_M03: Passed\n");
    } else {
        printf("testMcodeParse_M03: Failed\n");
    }
}

void testMcodeParse_M04() {
    char *mcode = "M04";
    char *args[] = {"S1500"};
    int len = 1;
    char curr_tool[10] = "n/a";
    char spindle_speed[10];

    int result = mcode_parse(mcode, args, len, curr_tool, spindle_speed);

    // Validate the behavior for M04 command
    if (result == 1 && strcmp(spindle_speed, "1500") == 0) {
        printf("testMcodeParse_M04: Passed\n");
    } else {
        printf("testMcodeParse_M04: Failed\n");
    }
}

void testMcodeParse_M06() {
    char *mcode = "M06";
    char *args[] = {"T3"};
    int len = 1;
    char curr_tool[10];
    char spindle_speed[10] = "n/a";

    int result = mcode_parse(mcode, args, len, curr_tool, spindle_speed);

    // Validate the behavior for M06 command
    if (result == 1 && strcmp(curr_tool, "3") == 0) {
        printf("testMcodeParse_M06: Passed\n");
    } else {
        printf("testMcodeParse_M06: Failed\n");
    }
}

void testMcodeParse_M16() {
    char *mcode = "M16";
    char *args[] = {"T5"};
    int len = 1;
    char curr_tool[10];
    char spindle_speed[10] = "n/a";

    int result = mcode_parse(mcode, args, len, curr_tool, spindle_speed);

    // Validate the behavior for M16 command
    if (result == 1 && strcmp(curr_tool, "5") == 0) {
        printf("testMcodeParse_M16: Passed\n");
    } else {
        printf("testMcodeParse_M16: Failed\n");
    }
}
