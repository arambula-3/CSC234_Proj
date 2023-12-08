#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "cutter_compensation_validate.c"

// Define a tolerance for floating-point comparison
#define FLOAT_TOL 0.001

// Function to check if two floats are approximately equal
int approx_equal(float a, float b, float tol) {
    return fabs(a - b) < tol;
}

void testCutterCompensation(char *testName, char *cutter_comp_direction, int comp_count, 
                            char *x_pos, char *y_pos, float d_len2, char *prev_x_pos, char *prev_y_pos,
                            float expected_x_comp, float expected_y_comp, char *recent_gcode, 
                            char *prev_x_pos2, char *prev_y_pos2, char *previous_gcode) {
    float x_comp = 0.0, y_comp = 0.0;
    int result = cutter_compensation_validate(cutter_comp_direction, comp_count, x_pos, y_pos,
                                              &d_len2, prev_x_pos, prev_y_pos, &x_comp, &y_comp,
                                              recent_gcode, prev_x_pos2, prev_y_pos2, previous_gcode);

    printf("Test: %s\n", testName);
    printf("Expected x_comp: %f, Actual x_comp: %f\n", expected_x_comp, x_comp);
    printf("Expected y_comp: %f, Actual y_comp: %f\n", expected_y_comp, y_comp);

    assert(result == 1); // Assuming successful execution returns 1
    assert(approx_equal(x_comp, expected_x_comp, FLOAT_TOL));

    printf("%s Passed\n", testName);
}
int main() {
    printf("Starting regression tests for cutter_compensation_validate...\n");

    // First test case
    testCutterCompensation("TestLeftStraight", "left", 1, "10", "20", 5.0, "5", "15",
                           7.5, 16.035534, "G01", "0", "10", "G00");

    // Adjusted second test case with updated expected values
    testCutterCompensation("TestRightArc", "right", 1, "15", "25", 5.0, "10", "20",
                           7.5, 20.0, "G02", "5", "15", "G01");


    printf("Regression tests completed.\n");
    return 0;
}
