#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output_print.c" 

// Test function declaration
void testOutputPrint();

int main() {
    printf("Starting output_print test cases...\n");

    testOutputPrint();

    printf("Finished testing output_print.\n");
    return 0;
}

void testOutputPrint() {
    FILE *fp = stdout; 

    // Set up other parameters
    int comp_pos = 1;
    char *recent_gcode = "G01";
    char *x_pos = "X10";
    char *y_pos = "Y20";
    char *z_pos = "Z5";
    char *i_pos = "I1";
    char *j_pos = "J2";
    char *curr_tool = "Tool1";
    char *spindle_speed = "5000";
    float x_final_pos = 15.0;
    float y_final_pos = 25.0;
    float x_comp_pos = 10.5;
    float y_comp_pos = 20.5;
    float d_len2 = 2.5;

    // Call output_print
    output_print(fp, comp_pos, recent_gcode, x_pos, y_pos, z_pos, i_pos,
                 j_pos, curr_tool, spindle_speed, x_final_pos, y_final_pos,
                 x_comp_pos, y_comp_pos, d_len2);

    printf("testOutputPrint: Completed (manual verification required)\n");
}
