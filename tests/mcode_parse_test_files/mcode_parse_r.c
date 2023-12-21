#include <stdio.h>
#include <assert.h>
#include "../../mcode_parse.c"  // Include the mcode_parse module

void test_mcode_parse_valid() {
    char *valid_mcode = "M03";
    char *args[] = {"S1000"};
    int len = 1;
    char curr_tool[10] = "M00";
    char spindle_speed[10] = "1000";

    int result = mcode_parse(valid_mcode, args, len, curr_tool, spindle_speed);
    assert(result != 0);  // Assuming non-zero indicates success
}

void test_mcode_parse_invalid() {
    char *invalid_mcode = "M99"; // Assuming M99 is an invalid code
    char *args[] = {"S1000"};
    int len = 1;
    char curr_tool[10] = "M04";
    char spindle_speed[10] = "1000";

    int result = mcode_parse(invalid_mcode, args, len, curr_tool, spindle_speed);
    assert(result == 0);  // Assuming 0 indicates failure
}

void test_mcode_parse_edge_case() {
    char *valid_mcode = "M03";
    char *args[] = {""}; // Edge case: empty argument
    int len = 1;
    char curr_tool[10] = "M06";
    char spindle_speed[10] = "";

    int result = mcode_parse(valid_mcode, args, len, curr_tool, spindle_speed);
    assert(result == 0);  // Edge case should fail
}

int main() {
    test_mcode_parse_valid();
    test_mcode_parse_invalid();
    test_mcode_parse_edge_case();
    printf("All regression tests passed.\n");
    return 0;
}
