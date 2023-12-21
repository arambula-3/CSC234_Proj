#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../cutter_compensation_validate.c" 

void testPathWithLeftCompensation();
void testPathWithRightCompensation();
void testPathWithInvalidCompensation();
int testCompensationCalculationLeft();
int testCompensationCalculationLeftArc();
int testCompensationCalculationRight360();

int main() {
    printf("Starting white box testing for cutter_compensation_validate...\n");

    testPathWithLeftCompensation();
    testPathWithRightCompensation();
    testPathWithInvalidCompensation();
    int num_passed_ccl = testCompensationCalculationLeft();
    int num_passed_ccla = testCompensationCalculationLeftArc();
    int num_passed_ccr360 = testCompensationCalculationRight360();
    printf("%d of 9 CompCalcLeft cases Passed\n", num_passed_ccl);
    printf("%d of 9 CompCalcLeftArc cases Passed\n", num_passed_ccla);
    printf("%d of 2 CompCalcRight360 cases Passed\n", num_passed_ccr360);

    printf("White box testing for cutter_compensation_validate completed.\n");
    return 0;
}

void testPathWithLeftCompensation() {
    
    char *cutter_comp_direction = "left";
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

    char *cutter_comp_direction = "right";
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
    int comp_count = -2;
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

int testCompensationCalculationLeft() {
    char *cutter_comp_direction = "left";
    int comp_count = 1;
    char x_pos[10] = "0";
    char y_pos[10] = "0";
    float d_len2 = 1.0;
    char prev_x_pos[10] = "1";
    char prev_y_pos[10] = "1";
    float x_comp_pos, y_comp_pos;
    char recent_gcode[10] = "G01";
    char prev_x_pos2[10] = "0";
    char prev_y_pos2[10] = "0";
    char previous_gcode[10] = "G01";
    char *rec_gcode_cases[] = {"G41", "G41", "G1","G1","G1","G1","G1","G1","G40"};
    char *prev_gcode_cases[] = {"G41", "G41","G41","G1","G1","G1","G1","G1","G1"};
    char *x_pos_cases[] = {"1","4","9","12","12","9","4","1","1"};
    char *y_pos_cases[] = {"9","12","12","9","5","2","2","5","1"};
    char *prev_x_pos_cases[] = {"1","1","4","9","12","12","9","4","1"};
    char *prev_y_pos_cases[] = {"5","9","12","12","9","5","2","2","5"};
    char *prev_x_pos2_cases[] = {"1","1","1","4","9","12","12","9","4"};
    char *prev_y_pos2_cases[] = {"1","5","9","12","12","9","5","2","2"};
    double x_comp_pos_cases[] = {.5,.5,3.792893f,9.207107f,12.5,12.5,9.207107f,3.792893f,.646447f};
    double y_comp_pos_cases[] = {5,9.207107f,12.5,12.5,9.207107f,4.792893f,1.5,1.5,4.646447};
  
    int track = 0;
    int num_passed = 0;

    while (track < 9) {
        strcpy(recent_gcode,"n/a");
        strcpy(previous_gcode,"n/a");
        strcpy(x_pos,"0");
        strcpy(y_pos,"0"); 
        strcpy(prev_x_pos, "n/a");
        strcpy(prev_y_pos, "n/a");
        strcpy(prev_x_pos2, "n/a");
        strcpy(prev_y_pos2, "n/a");

        strcpy(recent_gcode,rec_gcode_cases[track]);
        strcpy(previous_gcode,prev_gcode_cases[track]);
        strcpy(x_pos,x_pos_cases[track]);
        strcpy(y_pos,y_pos_cases[track]); 
        strcpy(prev_x_pos, prev_x_pos_cases[track]);
        strcpy(prev_y_pos, prev_y_pos_cases[track]);
        strcpy(prev_x_pos2, prev_x_pos2_cases[track]);
        strcpy(prev_y_pos2, prev_y_pos2_cases[track]);

        int result = 0;

        if (track != 8) {
            result = cutter_compensation_validate(cutter_comp_direction, comp_count,
                                                x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                                &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                                previous_gcode);
        } else {
            result = cutter_compensation_validate(cutter_comp_direction, -1,
                                                x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                                &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                                previous_gcode);
        }

        if (result == 1) {
            if (fabs(x_comp_pos_cases[track] - x_comp_pos) < .000001 && fabs(y_comp_pos_cases[track] - y_comp_pos) < .000001) {
                num_passed = num_passed + 1;
                printf("testCompCalcLeft: case %d Passed\n", track + 1);
            } else {
                printf("testCompCalcLeft: case %d Failed\n", track + 1);
            }
        } else {
            printf("testCompCalcLeft: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }

    return num_passed;
}

int testCompensationCalculationLeftArc() {
    char *cutter_comp_direction = "left";
    int comp_count = 1;
    char x_pos[10] = "0";
    char y_pos[10] = "0";
    float d_len2 = 1.0;
    char prev_x_pos[10] = "1";
    char prev_y_pos[10] = "1";
    float x_comp_pos, y_comp_pos;
    char recent_gcode[10] = "G01";
    char prev_x_pos2[10] = "0";
    char prev_y_pos2[10] = "0";
    char previous_gcode[10] = "G01";
    char *rec_gcode_cases[] = {"G41", "G2", "G1","G2","G1","G2","G1","G2","G40"};
    char *prev_gcode_cases[] = {"G41", "G41","G2","G1","G2","G1","G2","G1","G2"};
    char *x_pos_cases[] = {"1","4","9","12","12","9","4","1","1"};
    char *y_pos_cases[] = {"9","12","12","9","5","2","2","5","1"};
    char *prev_x_pos_cases[] = {"1","1","4","9","12","12","9","4","1"};
    char *prev_y_pos_cases[] = {"5","9","12","12","9","5","2","2","5"};
    char *prev_x_pos2_cases[] = {"1","1","1","4","9","12","12","9","4"};
    char *prev_y_pos2_cases[] = {"1","5","9","12","12","9","5","2","2"};
    double x_comp_pos_cases[] = {.5,.5,4,9,12.5,12.5,9,4,.646447f};
    double y_comp_pos_cases[] = {5,9,12.5,12.5,9,5,1.5,1.5,4.646447};
  
    int track = 0;
    int num_passed = 0;

    while (track < 9) {
        strcpy(recent_gcode,"n/a");
        strcpy(previous_gcode,"n/a");
        strcpy(x_pos,"0");
        strcpy(y_pos,"0"); 
        strcpy(prev_x_pos, "n/a");
        strcpy(prev_y_pos, "n/a");
        strcpy(prev_x_pos2, "n/a");
        strcpy(prev_y_pos2, "n/a");

        strcpy(recent_gcode,rec_gcode_cases[track]);
        strcpy(previous_gcode,prev_gcode_cases[track]);
        strcpy(x_pos,x_pos_cases[track]);
        strcpy(y_pos,y_pos_cases[track]); 
        strcpy(prev_x_pos, prev_x_pos_cases[track]);
        strcpy(prev_y_pos, prev_y_pos_cases[track]);
        strcpy(prev_x_pos2, prev_x_pos2_cases[track]);
        strcpy(prev_y_pos2, prev_y_pos2_cases[track]);

        int result = 0;

        if (track != 8) {
            result = cutter_compensation_validate(cutter_comp_direction, comp_count,
                                                x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                                &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                                previous_gcode);
        } else {
            result = cutter_compensation_validate(cutter_comp_direction, -1,
                                                x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                                &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                                previous_gcode);
        }

        if (result == 1) {
            if (fabs(x_comp_pos_cases[track] - x_comp_pos) < .000001 && fabs(y_comp_pos_cases[track] - y_comp_pos) < .000001) {
                num_passed = num_passed + 1;
                printf("testCompCalcLeftArc: case %d Passed\n", track + 1);
            } else {
                printf("testCompCalcLeftArc: case %d Failed\n", track + 1);
            }
        } else {
            printf("testCompCalcLeftArc: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }

    return num_passed;
}

int testCompensationCalculationRight360() {
    char *cutter_comp_direction = "right";
    int comp_count = 1;
    char x_pos[10] = "0";
    char y_pos[10] = "0";
    float d_len2 = 1.0;
    char prev_x_pos[10] = "1";
    char prev_y_pos[10] = "1";
    float x_comp_pos, y_comp_pos;
    char recent_gcode[10] = "G01";
    char prev_x_pos2[10] = "0";
    char prev_y_pos2[10] = "0";
    char previous_gcode[10] = "G01";
    char *rec_gcode_cases[] = {"G3", "G40"};
    char *prev_gcode_cases[] = {"G42","G3"};
    char *x_pos_cases[] = {"4","8"};
    char *y_pos_cases[] = {"1","1"};
    char *prev_x_pos_cases[] = {"4","4"};
    char *prev_y_pos_cases[] = {"1","1"};
    char *prev_x_pos2_cases[] = {"1","4"};
    char *prev_y_pos2_cases[] = {"1","1"};
    double x_comp_pos_cases[] = {4,4};
    double y_comp_pos_cases[] = {.5,.5};
  
    int track = 0;
    int num_passed = 0;

    while (track < 2) {
        strcpy(recent_gcode,"n/a");
        strcpy(previous_gcode,"n/a");
        strcpy(x_pos,"0");
        strcpy(y_pos,"0"); 
        strcpy(prev_x_pos, "n/a");
        strcpy(prev_y_pos, "n/a");
        strcpy(prev_x_pos2, "n/a");
        strcpy(prev_y_pos2, "n/a");

        strcpy(recent_gcode,rec_gcode_cases[track]);
        strcpy(previous_gcode,prev_gcode_cases[track]);
        strcpy(x_pos,x_pos_cases[track]);
        strcpy(y_pos,y_pos_cases[track]); 
        strcpy(prev_x_pos, prev_x_pos_cases[track]);
        strcpy(prev_y_pos, prev_y_pos_cases[track]);
        strcpy(prev_x_pos2, prev_x_pos2_cases[track]);
        strcpy(prev_y_pos2, prev_y_pos2_cases[track]);

        int result = 0;

        if (track != 1) {
            result = cutter_compensation_validate(cutter_comp_direction, comp_count,
                                                x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                                &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                                previous_gcode);
        } else {
            result = cutter_compensation_validate(cutter_comp_direction, -1,
                                                x_pos, y_pos, &d_len2, prev_x_pos, prev_y_pos,
                                                &x_comp_pos, &y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
                                                previous_gcode);
        }

        if (result == 1) {
            if (fabs(x_comp_pos_cases[track] - x_comp_pos) < .000001 && fabs(y_comp_pos_cases[track] - y_comp_pos) < .000001) {
                num_passed = num_passed + 1;
                printf("testCompCalcRight360: case %d Passed\n", track + 1);
            } else {
                printf("testCompCalcRight360: case %d Failed\n", track + 1);
            }
        } else {
            printf("testCompCalcRight360: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }

    return num_passed;
}
