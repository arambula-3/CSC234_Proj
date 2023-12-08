#include <stdio.h>
#include <stdlib.h>
#include "error_printer.c" // Assuming this is the correct path to the file

void testErrorPrinter(int testNum, int errorCode, int lineNumber) {
    printf("Running Test %d with errorCode=%d, lineNumber=%d\n", testNum, errorCode, lineNumber);

    // Call the error_printer function with test parameters
    // Direct output to the standard output (console)
    error_printer(stdout, errorCode, lineNumber);

    printf("Test %d completed.\n", testNum);
}

int main() {
    printf("Starting regression tests for error_printer...\n");

    // Example test cases
    testErrorPrinter(1, 1, 100); // Test with error code 1 at line 100
    testErrorPrinter(2, 2, 200); // Test with error code 2 at line 200
    testErrorPrinter(3, 3, 300); // Test with error code 3 at line 300
    testErrorPrinter(4, 0, 400); // Test with an unknown error code

    printf("Regression tests completed.\n");
    return 0;
}
