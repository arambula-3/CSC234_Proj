#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gcode_parse.c"  // Make sure this path is correct

// Function prototypes
void testGcodeParse_G00();
// Add prototypes for other test functions here

int main() {
    printf("Starting white box testing for gcode_parse...\n");

    testGcodeParse_G00();
    // Add calls to other test functions here

    printf("White box testing for gcode_parse completed.\n");
    return 0;
}

void testGcodeParse_G00() {
    char *gcode = "G00";
    char *coords[] = {"X10", "Y20"};
    int len = 2;
    int diameter_seen = 0;
    FILE *fp = tmpfile(); // Using a temporary file to capture output
    float d_len2 = 0.0;
    int comp_count = 0;
    char x_pos[10], y_pos[10], z_pos[10] = "0", feed_rate[10] = "n/a";
    char cutter_comp_direction[10] = "n/a", i_pos[10] = "n/a", j_pos[10] = "n/a", k_pos[10] = "n/a", r_pos[10] = "n/a";
    char recent_gcode[10] = "n/a", prev_x_pos[10] = "n/a", prev_y_pos[10] = "n/a";
    float x_comp_pos = 0.0, y_comp_pos = 0.0;
    char prev_x_pos2[10] = "n/a", prev_y_pos2[10] = "n/a", previous_gcode[10] = "n/a";

    int result = gcode_parse(gcode, coords, len, diameter_seen, fp, &d_len2, &comp_count,
                             x_pos, y_pos, z_pos, feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                             r_pos, recent_gcode, prev_x_pos, prev_y_pos, &x_comp_pos, &y_comp_pos,
                             prev_x_pos2, prev_y_pos2, previous_gcode);

    // Check the result - Assuming successful parsing for G00 returns 1
    if (result == 1) {
        printf("testGcodeParse_G00: Passed\n");
    } else {
        printf("testGcodeParse_G00: Failed\n");
    }

    fclose(fp);  // Close the temporary file
}

// Define more test functions for other G-code commands and scenarios
