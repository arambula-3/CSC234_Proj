#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_printer.c"  // Make sure this path is correct

// Test function declarations
void testErrorPrinter();

int main() {
    printf("Starting error_printer test cases...\n");

    testErrorPrinter();

    printf("Finished testing error_printer.\n");
    return 0;
}

void testErrorPrinter() {
    // Open a file for writing
    FILE *fp = fopen("C:/Users/rkolagotla/Downloads/withcuttercompensation_complexfaulty.txt", "w");
    if (fp == NULL) {
        perror("Unable to open file for writing");
        return;
    }

    // Test with a known error code and line number
    int errorCode = 1; // Assuming 1 is a valid error code
    int lineNumber = 100;
    error_printer(fp, errorCode, lineNumber);

    // Close the file after writing
    fclose(fp);

    // Reopen the file for reading
    fp = fopen("C:/Users/rkolagotla/Downloads/withcuttercompensation_complexfaulty.txt", "r");
    if (fp == NULL) {
        perror("Unable to open file for reading");
        return;
    }

    // Read the output from the file and verify it
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Adjust the expected output format according to your function's actual output
        char expectedOutput[256];
        sprintf(expectedOutput, "Error code %d occurred at line %d\n", errorCode, lineNumber);
        if (strcmp(buffer, expectedOutput) == 0) {
            printf("testErrorPrinter: Passed\n");
        } else {
            printf("testErrorPrinter: Failed - Output did not match expected result\n");
        }
    } else {
        printf("testErrorPrinter: Failed - No output read from file\n");
    }

    // Close the file
    fclose(fp);
}