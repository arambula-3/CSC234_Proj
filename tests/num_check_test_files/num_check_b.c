#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../num_check.c"  // Make sure this path is correct

// Test function declarations
void testNumCheck_ValidNumber();
void testNumCheck_InvalidNumber();

int main() {
    printf("Starting num_check test cases...\n");

    testNumCheck_ValidNumber();
    testNumCheck_InvalidNumber();

    printf("Finished testing num_check.\n");
    return 0;
}

void testNumCheck_ValidNumber() {
    // Set up a valid number string
    char *num_str = "123.45";

    // Call num_check
    int result = num_check(num_str);

    // Check the result - assuming the function returns 1 for valid numbers
    if (result == 1) {
        printf("testNumCheck_ValidNumber: Passed\n");
    } else {
        printf("testNumCheck_ValidNumber: Failed\n");
    }
}

void testNumCheck_InvalidNumber() {
    // Set up an invalid number string
    char *num_str = "abc123";

    // Call num_check
    int result = num_check(num_str);

    // Check the result - assuming the function returns 0 for invalid numbers
    if (result == 0) {
        printf("testNumCheck_InvalidNumber: Passed\n");
    } else {
        printf("testNumCheck_InvalidNumber: Failed\n");
    }
}
