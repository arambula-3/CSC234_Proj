#include <stdio.h>
#include <assert.h>
#include "num_check.c"  // Include the num_check module

void test_num_check_valid_float() {
    char *valid_float = "123.456";
    int result = num_check(valid_float);
    assert(result == 1);  // Expect 1 for valid float
}

void test_num_check_invalid_float() {
    char *invalid_float = "abc123";
    int result = num_check(invalid_float);
    assert(result == 0);  // Expect 0 for invalid float
}

void test_num_check_edge_cases() {
    // Test with an empty string
    char *empty_str = "";
    int result_empty = num_check(empty_str);
    assert(result_empty == 0);  // Expect 0 for empty string

    // Test with a string containing only a dot
    char *only_dot = ".";
    int result_dot = num_check(only_dot);
    assert(result_dot == 0);  // Expect 0 for string with only a dot
}

int main() {
    test_num_check_valid_float();
    test_num_check_invalid_float();
    test_num_check_edge_cases();
    printf("All regression tests passed.\n");
    return 0;
}
