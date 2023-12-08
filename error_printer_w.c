#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error_printer.c"

// Test function declarations
void testErrorPrinterWithCode(int errorCode, int lineNumber, const char *expectedOutput);

int main() {
    printf("white box testing for error_printer starts...\n");

    // Test with known error codes and expected outputs
    testErrorPrinterWithCode(1, 100, "improper proper code inputted at line = 100\n\n");
    testErrorPrinterWithCode(2, 200, "too many M codes on line = 200\n\n");
    testErrorPrinterWithCode(3, 300, "improper comment at line = 300\n\n");

    // Test with an unrecognized error code
    testErrorPrinterWithCode(999, 400, "");  // Assuming the function does not print anything for unrecognized codes

    printf("White box testing for error_printer completed.\n");
    return 0;
}

void testErrorPrinterWithCode(int errorCode, int lineNumber, const char *expectedOutput) {
    FILE *fp = tmpfile();
    if (fp == NULL) {
        perror("Unable to open temporary file");
        return;
    }

    error_printer(fp, errorCode, lineNumber);

    // Seek back to the beginning of the temporary file to read
    rewind(fp);

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strcmp(buffer, expectedOutput) == 0) {
            printf("testErrorPrinterWithCode %d: Passed\n", errorCode);
        } else {
            printf("testErrorPrinterWithCode %d: Failed - Output did not match expected result\n", errorCode);
        }
    } else if (strlen(expectedOutput) == 0) {
        printf("testErrorPrinterWithCode %d: Passed (No output as expected)\n", errorCode);
    } else {
        printf("testErrorPrinterWithCode %d: Failed - No output read from file\n", errorCode);
    }

    fclose(fp);
}
