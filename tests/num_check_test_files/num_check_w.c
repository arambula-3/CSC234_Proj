#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../num_check.c"  // Include the num_check function

// Test function declarations
void testNumCheck_ValidFloat();
void testNumCheck_InvalidFloat();
void testNumCheck_EmptyString();
void testNumCheck_NonNumericString();

int main() {
    printf("Starting white box testing for num_check...\n");

    testNumCheck_ValidFloat();
    testNumCheck_InvalidFloat();
    testNumCheck_EmptyString();
    testNumCheck_NonNumericString();

    printf("White box testing for num_check completed.\n");
    return 0;
}

void testNumCheck_ValidFloat() {
    char *valid_float = "123.456";
    int result = num_check(valid_float);

    if (result == 1) {
        printf("testNumCheck_ValidFloat: Passed\n");
    } else {
        printf("testNumCheck_ValidFloat: Failed\n");
    }
}

void testNumCheck_InvalidFloat() {
    char *invalid_float = "123.abc";
    int result = num_check(invalid_float);

    if (result == 0) {
        printf("testNumCheck_InvalidFloat: Passed\n");
    } else {
        printf("testNumCheck_InvalidFloat: Failed\n");
    }
}

void testNumCheck_EmptyString() {
    char *empty_string = "";
    int result = num_check(empty_string);

    if (result == 0) {
        printf("testNumCheck_EmptyString: Passed\n");
    } else {
        printf("testNumCheck_EmptyString: Failed\n");
    }
}

void testNumCheck_NonNumericString() {
    char *non_numeric = "Hello123";
    int result = num_check(non_numeric);

    if (result == 0) {
        printf("testNumCheck_NonNumericString: Passed\n");
    } else {
        printf("testNumCheck_NonNumericString: Failed\n");
    }
}
