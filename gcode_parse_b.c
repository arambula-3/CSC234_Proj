#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gcode_parse.c" 

// Test function declarations
void testGcodeParse_ValidInput();
void testGcodeParse_InvalidInput();

int main() {
    printf("Starting gcode_parse test cases...\n");

    testGcodeParse_ValidInput();
    testGcodeParse_InvalidInput();

    printf("Finished testing gcode_parse.\n");
    return 0;
}

void testGcodeParse_ValidInput() {
    // Set up a valid G-code input and other parameters
    char *gcode = "G01";
    char *coords[] = {"X10", "Y10"};
    int len = 2;
    int diameter_seen = 0;
    FILE *fp = stdout; // Assuming standard output for simplicity
    float d_len2 = 0.0;
    int comp_count = 0;
    char x_pos[10], y_pos[10], z_pos[10], feed_rate[10], cutter_comp_direction[10];
    char i_pos[10], j_pos[10], k_pos[10], r_pos[10], recent_gcode[10], prev_x_pos[10], prev_y_pos[10];
    float x_comp_pos = 0.0, y_comp_pos = 0.0;
    char prev_x_pos2[10], prev_y_pos2[10], previous_gcode[10];

    // Initialize some parameter values
    strcpy(x_pos, "X10");
    strcpy(y_pos, "Y10");
    strcpy(recent_gcode, "G00");

    // Call gcode_parse
    int result = gcode_parse(gcode, coords, len, diameter_seen, fp, &d_len2, &comp_count,
                             x_pos, y_pos, z_pos, feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                             r_pos, recent_gcode, prev_x_pos, prev_y_pos, &x_comp_pos, &y_comp_pos,
                             prev_x_pos2, prev_y_pos2, previous_gcode);

    // Check the result - assuming successful parsing returns 1
    if (result == 1) {
        printf("testGcodeParse_ValidInput: Passed\n");
    } else {
        printf("testGcodeParse_ValidInput: Failed\n");
    }
}

void testGcodeParse_InvalidInput() {
    // Set up an invalid G-code input and other parameters
    char *gcode = "Invalid";
    char *coords[] = {"X-1", "Y-1"};
    int len = 2;
    int diameter_seen = 0;
    FILE *fp = stdout; // Assuming standard output for simplicity
    float d_len2 = 0.0;
    int comp_count = 0;
    char x_pos[10], y_pos[10], z_pos[10], feed_rate[10], cutter_comp_direction[10];
    char i_pos[10], j_pos[10], k_pos[10], r_pos[10], recent_gcode[10], prev_x_pos[10], prev_y_pos[10];
    float x_comp_pos = 0.0, y_comp_pos = 0.0;
    char prev_x_pos2[10], prev_y_pos2[10], previous_gcode[10];

    // Initialize some parameter values
    strcpy(x_pos, "X-1");
    strcpy(y_pos, "Y-1");
    strcpy(recent_gcode, "G00");

    // Call gcode_parse
    int result = gcode_parse(gcode, coords, len, diameter_seen, fp, &d_len2, &comp_count,
                             x_pos, y_pos, z_pos, feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                             r_pos, recent_gcode, prev_x_pos, prev_y_pos, &x_comp_pos, &y_comp_pos,
                             prev_x_pos2, prev_y_pos2, previous_gcode);

    // Check the result - assuming failure returns 0
    if (result == 0) {
        printf("testGcodeParse_InvalidInput: Passed\n");
    } else {
        printf("testGcodeParse_InvalidInput: Failed\n");
    }
}
