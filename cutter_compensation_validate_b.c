#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cutter_compensation_validate.c" 

// Test function declarations
void testCutterCompensationValidate_ValidInput();
void testCutterCompensationValidate_InvalidInput();

int main() {
    printf("Starting cutter_compensation_validate test cases...\n");

    testCutterCompensationValidate_ValidInput();
    testCutterCompensationValidate_InvalidInput();

    printf("Finished testing cutter_compensation_validate.\n");
    return 0;
}

void testCutterCompensationValidate_ValidInput() {
    // Set up valid inputs
    char *cutter_comp_direction = "left";
    int comp_count = 1;
    char *x_pos = "10";
    char *y_pos = "20";
    float d_len2 = 5.0;
    char *prev_x_pos = "5";
    char *prev_y_pos = "15";
    float x_comp_pos = 10.5;
    float y_comp_pos = 20.5;
    char *recent_gcode = "G01";
    char *prev_x_pos2 = "0";
    char *prev_y_pos2 = "10";
    char *previous_gcode = "G00";

    // Call cutter_compensation_validate
    int result = cutter_compensation_validate(cutter_comp_direction, comp_count,
                                              x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                              &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                              previous_gcode);

    // Check the result - assuming successful validation returns 1
    if (result == 1) {
        printf("testCutterCompensationValidate_ValidInput: Passed\n");
    } else {
        printf("testCutterCompensationValidate_ValidInput: Failed\n");
    }
}

void testCutterCompensationValidate_InvalidInput() {
    // Set up invalid inputs
    char *cutter_comp_direction = "Invalid";
    int comp_count = -1;
    char *x_pos = "-10";
    char *y_pos = "-20";
    float d_len2 = 0;
    char *prev_x_pos = "-5";
    char *prev_y_pos = "-15";
    float x_comp_pos = -10.5;
    float y_comp_pos = -20.5;
    char *recent_gcode = "G02";
    char *prev_x_pos2 = "-1";
    char *prev_y_pos2 = "-10";
    char *previous_gcode = "G01";

    // Call cutter_compensation_validate
    int result = cutter_compensation_validate(cutter_comp_direction, comp_count,
                                              x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                              &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                              previous_gcode);

    // Check the result - assuming failed validation returns 0
    if (result == 0) {
        printf("testCutterCompensationValidate_InvalidInput: Passed\n");
    } else {
        printf("testCutterCompensationValidate_InvalidInput: Failed\n");
    }
}