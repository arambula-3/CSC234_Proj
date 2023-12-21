#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../rest_token_creator.c" // Include the file with the function

void print_tokens(char *tokens[], int size) {
    for (int i = 0; i < size; i++) {
        printf("'%s' ", tokens[i]);
    }
    printf("\\n");
}

int test_case(char *test_description, char *input, char *expected[], int expected_index, int expected_in_comment, int mode) {
    char *rest_token[BUFFERSIZE];
    int index = 0;
    int in_comment = 0;
    int arr[2];

    // Create a copy of the input as strtok modifies it
    char input_copy[BUFFERSIZE];
    strcpy(input_copy, input);

    char *token = strtok(input_copy, " ");
    rest_token_creator(token, rest_token, index, in_comment, arr, mode);

    // Check if the test passed
    int passed = 1;
    if (arr[0] != expected_index || arr[1] != expected_in_comment) {
        passed = 0;
    } else {
        for (int i = 0; i < expected_index; i++) {
            if (strcmp(rest_token[i], expected[i]) != 0) {
                passed = 0;
                break;
            }
        }
    }

    printf("Test '%s': %s\n", test_description, passed ? "Passed" : "Failed");
    if (!passed) {
        printf("Expected: ");
        print_tokens(expected, expected_index);
        printf("Got: ");
        print_tokens(rest_token, arr[0]);
    }

    return passed;
}

int main() {
    int total_tests = 0;
    int passed_tests = 0;

    // Test Case 1: Simple token parsing
    char *input1 = "G01 X10 Y39 Z45.09 F100";
    char *expected1[] = {"G01", "X10", "Y39", "Z45.09","F100"};
    total_tests++;
    passed_tests += test_case("Simple token parsing", input1, expected1, 5, 0, 1);

    // Test Case 2: Handling comments
    char *input2 = "G03 X300 Y250 I25 J50 F100 (Counter-clockwise arc);";
    char *expected2[] = {"G03", "X300", "Y250", "I25", "J50", "F100"};
    total_tests++;
    passed_tests += test_case("Handling comments", input2, expected2, 6, 0, 1);

    //Test Case 3: Token parsing another test case
    char *input3 = "G02 X5 Y 5 Z9.0 F95";
    char *expected3[] = {"G02", "X5", "Y5", "Z9.0", "F95"};
    total_tests++;
    passed_tests += test_case("3rd test case", input3,expected3, 5, 0, 2);

    printf("\\nTotal Tests: %d, Passed: %d, Failed: %d\n", total_tests, passed_tests, total_tests - passed_tests);
    return 0;
}
