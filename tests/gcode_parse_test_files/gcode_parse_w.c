#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../gcode_parse.c"  // Make sure this path is correct
#include <assert.h>

// Function prototypes
int testGcodeParse_G00();
int testGcodeParse_G01();
int testGcodeParse_G02_3();
int testGcodeParse_G40();
int testGcodeParse_G41_42();
// Add prototypes for other test functions here

int main() {
    printf("Starting white box testing for gcode_parse...\n");

    int num_g0_pass = testGcodeParse_G00();
    int num_g1_pass = testGcodeParse_G01();
    int num_g2_3_pass = testGcodeParse_G02_3();
    int num_g40_pass = testGcodeParse_G40();
    int num_g41_42_pass = testGcodeParse_G41_42();
    printf("%d of 12 G0 cases Passed\n", num_g0_pass);
    printf("%d of 11 G1 cases Passed\n", num_g1_pass);
    printf("%d of 14 G2/3 cases Passed\n", num_g2_3_pass);
    printf("%d of 12 G40 cases Passed\n", num_g40_pass);
    printf("%d of 13 G41/42 cases Passed\n", num_g41_42_pass);

    printf("White box testing for gcode_parse completed.\n");
    return 0;
}

int testGcodeParse_G00() {
    char *case1[] = {"X100", "Y50", "Z10"};
    char *case2[] = {"X-20", "Y-30."};
    char *case3[] = {"Z25"};
    char *case4[] = {"X.0", "Y0", "Z0"};
    char *case5[] = {"X30", "Y2.0"};
    char *case6[] = {"X10", "Z15", "Y5;"};
    char *case7[] = {"X15", "Y20", "Z5;"};
    char *case8[] = {"h"}; //error
    char *case9[] = {"X"}; //error
    char *case10[] = {"Yh"}; //error
    char *case11[] = {""}; //error
    char *case12[] = {" "}; //error
    char *unit_tests[] = {case1, case2, case3, case4, case5, case6, case7, case8, case9, case10, case11, case12};

    int *case_len[] = {3,2,1,3,2,3,3,1,1,1,1,1};

    FILE *fp = fopen("../test.txt", "w"); // Using a temporary file to capture output
    char *gcode = "G00";
    int diameter_seen = 0;
    float d_len2 = 0.0;
    int comp_count = 0;
    char x_pos[10] = "0", y_pos[10] = "0", z_pos[10] = "0", feed_rate[10] = "n/a";
    char cutter_comp_direction[10] = "n/a", i_pos[10] = "n/a", j_pos[10] = "n/a", k_pos[10] = "n/a", r_pos[10] = "n/a";
    char recent_gcode[10] = "n/a", prev_x_pos[10] = "n/a", prev_y_pos[10] = "n/a";
    float x_comp_pos = 0.0, y_comp_pos = 0.0;
    char prev_x_pos2[10] = "n/a", prev_y_pos2[10] = "n/a", previous_gcode[10] = "n/a";
    int track = 0;
    int num_passed = 0;

    while (track < 12) {
        strcpy(x_pos,"0");
        strcpy(y_pos,"0"); 
        strcpy(z_pos, "0");
        strcpy(prev_x_pos, "n/a");
        strcpy(prev_y_pos, "n/a");

        int result = gcode_parse(gcode, unit_tests[track], case_len[track], diameter_seen, fp, &d_len2, &comp_count,
                             x_pos, y_pos, z_pos, feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                             r_pos, recent_gcode, prev_x_pos, prev_y_pos, &x_comp_pos, &y_comp_pos,
                             prev_x_pos2, prev_y_pos2, previous_gcode);
        if ((track < 7 && result == 1) || (track >= 7 && result == 0)) {
            num_passed = num_passed + 1;
            printf("testGcodeParse_G00: case %d Passed\n", track + 1);
        } else {
            printf("testGcodeParse_G00: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }
    fclose(fp);  // Close the temporary file
    return num_passed;
}

int testGcodeParse_G01() {
    char *case1[] = {"X10", "Y5", "Z2", "F200;"};
    char *case2[] = {"Y5", "X-7.0", "F23"};
    char *case3[] = {"F25", "Z25"};
    char *case4[] = {"F300;"};
    char *case5[] = {"X10", "Z15", "F20", "I5"}; //error
    char *case6[] = {"F"}; //error
    char *case7[] = {"Fh"}; //error
    char *case8[] = {"X"}; //error
    char *case9[] = {"Yh"}; //error
    char *case10[] = {""}; //error
    char *case11[] = {" "}; //error
    char *unit_tests[] = {case1, case2, case3, case4, case5, case6, case7, case8, case9, case10, case11};

    int *case_len[] = {4,3,2,1,4,1,1,1,1,1,1};

    FILE *fp = fopen("../test.txt", "w"); // Using a temporary file to capture output
    char *gcode = "G01";
    int diameter_seen = 0;
    float d_len2 = 0.0;
    int comp_count = 0;
    char x_pos[10] = "0", y_pos[10] = "0", z_pos[10] = "0", feed_rate[10] = "n/a";
    char cutter_comp_direction[10] = "n/a", i_pos[10] = "n/a", j_pos[10] = "n/a", k_pos[10] = "n/a", r_pos[10] = "n/a";
    char recent_gcode[10] = "n/a", prev_x_pos[10] = "n/a", prev_y_pos[10] = "n/a";
    float x_comp_pos = 0.0, y_comp_pos = 0.0;
    char prev_x_pos2[10] = "n/a", prev_y_pos2[10] = "n/a", previous_gcode[10] = "n/a";
    int track = 0;
    int num_passed = 0;

    while (track < 11) {
        strcpy(x_pos,"0");
        strcpy(y_pos,"0"); 
        strcpy(z_pos, "0");
        strcpy(prev_x_pos, "n/a");
        strcpy(prev_y_pos, "n/a");
        strcpy(i_pos, "n/a");
        strcpy(j_pos, "n/a");
        strcpy(feed_rate, "n/a");

        int result = gcode_parse(gcode, unit_tests[track], case_len[track], diameter_seen, fp, &d_len2, &comp_count,
                             x_pos, y_pos, z_pos, feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                             r_pos, recent_gcode, prev_x_pos, prev_y_pos, &x_comp_pos, &y_comp_pos,
                             prev_x_pos2, prev_y_pos2, previous_gcode);
        if ((track < 4 && result == 1) || (track >= 4 && result == 0)) {
            printf("testGcodeParse_G01: case %d Passed\n", track + 1);
            num_passed = num_passed + 1;
        } else {
            printf("testGcodeParse_G01: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }
    fclose(fp);  // Close the temporary file
    return num_passed;
}

int testGcodeParse_G02_3() {
    char *case1[] = {"X1", "Y1", "F30", "I1", "J0;"};
    char *case2[] = {"X1", "Y-1", "J-1", "F916"};
    char *case3[] = {"J1", "F50"};
    char *case4[] = {"X1", "Y-1", "J0", "F916"}; //error
    char *case5[] = {"X1", "Y-1", "K-1", "F916"}; //error
    char *case6[] = {"X1", "Y-1", "R-1", "F916"}; //error
    char *case7[] = {"X1", "Y1", "I1"}; //error
    char *case8[] = {"X10", "Z15", "Y5;", "I-50"}; //error
    char *case9[] = {"X15", "Y20", "Z5;"}; //error
    char *case10[] = {"J"}; //error
    char *case11[] = {"X"}; //error
    char *case12[] = {"Yh"}; //error
    char *case13[] = {""}; //error
    char *case14[] = {" "}; //error
    char *unit_tests[] = {case1, case2, case3, case4, case5, case6, case7, case8, case9, case10, case11, case12, case13, case14};

    int *case_len[] = {5,4,2,4,4,4,3,4,3,1,1,1,1,1};

    FILE *fp = fopen("../test.txt", "w"); // Using a temporary file to capture output
    char *gcode = "G02";
    int diameter_seen = 0;
    float d_len2 = 0.0;
    int comp_count = 0;
    char x_pos[10] = "0", y_pos[10] = "0", z_pos[10] = "0", feed_rate[10] = "n/a";
    char cutter_comp_direction[10] = "n/a", i_pos[10] = "n/a", j_pos[10] = "n/a", k_pos[10] = "n/a", r_pos[10] = "n/a";
    char recent_gcode[10] = "G02", prev_x_pos[10] = "0", prev_y_pos[10] = "0";
    float x_comp_pos = 0.0, y_comp_pos = 0.0;
    char prev_x_pos2[10] = "n/a", prev_y_pos2[10] = "n/a", previous_gcode[10] = "n/a";
    int track = 0;
    int num_passed = 0;

    while (track < 14) {
        strcpy(x_pos,"0");
        strcpy(y_pos,"0"); 
        strcpy(z_pos, "0");
        strcpy(prev_x_pos, "n/a");
        strcpy(prev_y_pos, "n/a");
        strcpy(i_pos, "n/a");
        strcpy(j_pos, "n/a");
        strcpy(feed_rate, "n/a");

        int result = gcode_parse(gcode, unit_tests[track], case_len[track], diameter_seen, fp, &d_len2, &comp_count,
                             x_pos, y_pos, z_pos, feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                             r_pos, recent_gcode, prev_x_pos, prev_y_pos, &x_comp_pos, &y_comp_pos,
                             prev_x_pos2, prev_y_pos2, previous_gcode);

        if ((track < 3 && result == 1) || (track >= 3 && result == 0)) {
            printf("testGcodeParse_G02_3: case %d Passed\n", track + 1);
            num_passed = num_passed + 1;
        } else {
            printf("testGcodeParse_G02_3: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }
    fclose(fp);  // Close the temporary file
    return num_passed;
}

int testGcodeParse_G40() {
    char *case1[] = {"X1", "Y1"};
    char *case2[] = {"X2;"};
    char *case3[] = {"Y3"};
    char *case4[] = {"X1", "Y1", "J1"}; //error
    char *case5[] = {"X10", "Z15", "Y5", "I-50"}; //error
    char *case6[] = {"X15", "Y20", "Z5;"}; //error
    char *case7[] = {"X1", "Y1", "F20"}; //error
    char *case8[] = {"J"}; //error
    char *case9[] = {"X"}; //error
    char *case10[] = {"Yh"}; //error
    char *case11[] = {""}; //error
    char *case12[] = {" "}; //error
    char *unit_tests[] = {case1, case2, case3, case4, case5, case6, case7, case8, case9, case10, case11, case12};

    int *case_len[] = {2,1,1,3,4,3,3,1,1,1,1,1};

    FILE *fp = fopen("../test.txt", "w"); // Using a temporary file to capture output
    char *gcode = "G40";
    int diameter_seen = 0;
    float d_len2 = 0.0;
    int comp_count = 0;
    char x_pos[10] = "0", y_pos[10] = "0", z_pos[10] = "0", feed_rate[10] = "n/a";
    char cutter_comp_direction[10] = "n/a", i_pos[10] = "n/a", j_pos[10] = "n/a", k_pos[10] = "n/a", r_pos[10] = "n/a";
    char recent_gcode[10] = "G40", prev_x_pos[10] = "0", prev_y_pos[10] = "0";
    float x_comp_pos = 0.0, y_comp_pos = 0.0;
    char prev_x_pos2[10] = "n/a", prev_y_pos2[10] = "n/a", previous_gcode[10] = "n/a";
    int track = 0;
    int num_passed = 0;

    while (track < 12) {
        strcpy(x_pos,"0");
        strcpy(y_pos,"0"); 
        strcpy(z_pos, "0");
        strcpy(prev_x_pos, "n/a");
        strcpy(prev_y_pos, "n/a");
        strcpy(i_pos, "n/a");
        strcpy(j_pos, "n/a");
        strcpy(feed_rate, "n/a");

        int result = gcode_parse(gcode, unit_tests[track], case_len[track], diameter_seen, fp, &d_len2, &comp_count,
                             x_pos, y_pos, z_pos, feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                             r_pos, recent_gcode, prev_x_pos, prev_y_pos, &x_comp_pos, &y_comp_pos,
                             prev_x_pos2, prev_y_pos2, previous_gcode);

        if ((track < 3 && result == 1) || (track >= 3 && result == 0)) {
            printf("testGcodeParse_G40: case %d Passed\n", track + 1);
            num_passed = num_passed + 1;
        } else {
            printf("testGcodeParse_G40: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }
    fclose(fp);  // Close the temporary file
    return num_passed;
}

int testGcodeParse_G41_42() {
    char *case1[] = {"X1", "Y1", "F23", "D5;"};
    char *case2[] = {"X2", "F63", "D9"};
    char *case3[] = {"Y3" , "D4"};
    char *case4[] = {"Y5" , "F93"}; //error
    char *case5[] = {"X1", "Y1", "J1"}; //error
    char *case6[] = {"X10", "Z15", "Y5", "I-50"}; //error
    char *case7[] = {"X15", "Y20", "Z5;"}; //error
    char *case8[] = {"X1", "Y1", "F20"}; //error
    char *case9[] = {"J"}; //error
    char *case10[] = {"X"}; //error
    char *case11[] = {"Yh"}; //error
    char *case12[] = {""}; //error
    char *case13[] = {" "}; //error
    char *unit_tests[] = {case1, case2, case3, case4, case5, case6, case7, case8, case9, case10, case11, case12, case13};

    int *case_len[] = {4,3,2,2,3,4,3,3,1,1,1,1,1};

    FILE *fp = fopen("../test.txt", "w"); // Using a temporary file to capture output
    char *gcode = "G41";
    int diameter_seen = 0;
    float d_len2 = 0.0;
    int comp_count = 0;
    char x_pos[10] = "0", y_pos[10] = "0", z_pos[10] = "0", feed_rate[10] = "n/a";
    char cutter_comp_direction[10] = "n/a", i_pos[10] = "n/a", j_pos[10] = "n/a", k_pos[10] = "n/a", r_pos[10] = "n/a";
    char recent_gcode[10] = "G41", prev_x_pos[10] = "0", prev_y_pos[10] = "0";
    float x_comp_pos = 0.0, y_comp_pos = 0.0;
    char prev_x_pos2[10] = "n/a", prev_y_pos2[10] = "n/a", previous_gcode[10] = "n/a";
    int track = 0;
    int num_passed = 0;

    while (track < 13) {
        strcpy(x_pos,"0");
        strcpy(y_pos,"0"); 
        strcpy(z_pos, "0");
        strcpy(prev_x_pos, "n/a");
        strcpy(prev_y_pos, "n/a");
        strcpy(i_pos, "n/a");
        strcpy(j_pos, "n/a");
        strcpy(feed_rate, "n/a");
        d_len2 = 0;

        int result = gcode_parse(gcode, unit_tests[track], case_len[track], diameter_seen, fp, &d_len2, &comp_count,
                             x_pos, y_pos, z_pos, feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                             r_pos, recent_gcode, prev_x_pos, prev_y_pos, &x_comp_pos, &y_comp_pos,
                             prev_x_pos2, prev_y_pos2, previous_gcode);

        if ((track < 3 && result == 1) || (track >= 3 && result == 0)) {
            printf("testGcodeParse_G41_42: case %d Passed\n", track + 1);
            num_passed = num_passed + 1;
        } else {
            printf("testGcodeParse_G41_42: case %d Failed\n", track + 1);
        }
        track = track + 1;
    }
    fclose(fp);  // Close the temporary file
    return num_passed;
}
