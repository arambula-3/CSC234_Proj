#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../mcode_parse.c"  // Make sure this path is correct

// Test function declarations
void testMcodeParse_ValidInput();
void testMcodeParse_InvalidInput();

int main() {
    printf("Starting mcode_parse test cases...\n");

    testMcodeParse_ValidInput();
    testMcodeParse_InvalidInput();

    printf("Finished testing mcode_parse.\n");
    return 0;
}

void testMcodeParse_ValidInput() {
    // Set up a valid M-code input and other parameters
    char *mcode = "M03";
    char *args[] = {"S1200"};
    int len = 1;
    char curr_tool[10] = "Tool1";
    char spindle_speed[10] = "1200";

    // Call mcode_parse
    int result = mcode_parse(mcode, args, len, curr_tool, spindle_speed);

    // Check the result - assuming successful parsing returns 1
    if (result == 1) {
        printf("testMcodeParse_ValidInput: Passed\n");
    } else {
        printf("testMcodeParse_ValidInput: Failed\n");
    }
}

void testMcodeParse_InvalidInput() {
    // Set up an invalid M-code input and other parameters
    char *mcode = "Invalid";
    char *args[] = {"S-1"};
    int len = 1;
    char curr_tool[10] = "Tool1";
    char spindle_speed[10] = "-1";

    // Call mcode_parse
    int result = mcode_parse(mcode, args, len, curr_tool, spindle_speed);

    // Check the result - assuming failure returns 0
    if (result == 0) {
        printf("testMcodeParse_InvalidInput: Passed\n");
    } else {
        printf("testMcodeParse_InvalidInput: Failed\n");
    }
}
