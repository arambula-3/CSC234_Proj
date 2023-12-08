#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cutter_compensation_validate.c" 

void testPathWithLeftCompensation();
void testPathWithRightCompensation();
void testPathWithInvalidCompensation();

int main() {
    printf("Starting white box testing for cutter_compensation_validate...\n");

    testPathWithLeftCompensation();
    testPathWithRightCompensation();
    testPathWithInvalidCompensation();

    printf("White box testing for cutter_compensation_validate completed.\n");
    return 0;
}

void testPathWithLeftCompensation() {
    
    char *cutter_comp_direction = "G41";
    int comp_count = 1;
    char *x_pos = "10";
    char *y_pos = "20";
    float d_len2 = 5.0;
    char *prev_x_pos = "5";
    char *prev_y_pos = "15";
    float x_comp_pos, y_comp_pos;
    char *recent_gcode = "G01";
    char *prev_x_pos2 = "0";
    char *prev_y_pos2 = "10";
    char *previous_gcode = "G00";
    
    int result = cutter_compensation_validate(cutter_comp_direction, comp_count,
                                              x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                              &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                              previous_gcode);

    printf("testPathWithLeftCompensation %s\n", result == 1 ? "passed" : "failed");
}

void testPathWithRightCompensation() {

    char *cutter_comp_direction = "G42";
    int comp_count = 1;
    char *x_pos = "15";
    char *y_pos = "25";
    float d_len2 = 5.0;
    char *prev_x_pos = "10";
    char *prev_y_pos = "20";
    float x_comp_pos, y_comp_pos;
    char *recent_gcode = "G01";
    char *prev_x_pos2 = "5";
    char *prev_y_pos2 = "15";
    char *previous_gcode = "G00";

    int result = cutter_compensation_validate(cutter_comp_direction, comp_count,
                                              x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                              &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                              previous_gcode);

    printf("testPathWithRightCompensation %s\n", result == 1 ? "passed" : "failed");
}

void testPathWithInvalidCompensation() {

    char *cutter_comp_direction = "InvalidDirection";
    int comp_count = -1;
    char *x_pos = "-10";
    char *y_pos = "-20";
    float d_len2 = -5.0;
    char *prev_x_pos = "-5";
    char *prev_y_pos = "-15";
    float x_comp_pos = -10.5;
    float y_comp_pos = -20.5;
    char *recent_gcode = "Invalid";
    char *prev_x_pos2 = "-1";
    char *prev_y_pos2 = "-10";
    char *previous_gcode = "Invalid";

    int result = cutter_compensation_validate(cutter_comp_direction, comp_count,
                                              x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                              &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                              previous_gcode);
                                              
    printf("testPathWithInvalidCompensation %s\n", result == 0 ? "passed" : "failed");
}
