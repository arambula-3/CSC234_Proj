#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../output_print.c"

// Function to process test data from a file
int intermediatePointsStraightLine();
int intermediatePointsArc();

int main() {
    printf("Starting file-based white box testing for output_print...\n");

    int ipsl_check = intermediatePointsStraightLine();
    int ipa_check = intermediatePointsArc();
    if (ipsl_check == 1) {
        printf("Testing for straight line intermediate points Passed.\n");
    } else {
        printf("Testing for straight line intermediate points Failed.\n");
    }
    if (ipa_check == 1) {
        printf("Testing for arc intermediate points Passed.\n");
    } else {
        printf("Testing for arc intermediate points Failed.\n");
    }

    printf("File-based white box testing for output_print completed.\n");
    return 0;
}

int intermediatePointsStraightLine() {
    FILE *fp1 = fopen("../../test.txt", "w");
    FILE *fp2 = fopen("../../test_texts/output_print_w_sl_valid.txt","r");
    if (fp1 == NULL) {
        perror("Error opening test data file");
        return;
    }
    if (fp2 == NULL) {
        perror("Error opening test data file");
        return;
    }

    double x_comp_pos_cases[] = {2,2,-2,0};
    double y_comp_pos_cases[] = {0,1,1,-2};
    char *z_pos_cases[] = {"0","-1","0","-.5"};

    float d_len2 = 0.0;
    int comp_pos = 1;
    char x_pos[10] = "0", y_pos[10] = "0", z_pos[10] = "0";
    char cutter_comp_direction[10] = "n/a", i_pos[10] = "n/a", j_pos[10] = "n/a";
    char recent_gcode[10] = "G1", prev_z_pos[10] = "n/a";
    char prev_i_pos[10] = "n/a", prev_j_pos[10] = "n/a", curr_tool[10] = "1", spindle_speed[10] = "916";
    float x_final_pos = 0, y_final_pos = 0;
    char previous_gcode[10] = "n/a";
    int track = 0;

    while (track < 4) {
        strcpy(x_pos, "0");
        strcpy(y_pos, "0");
        strcpy(z_pos,"0");
        strcpy(prev_z_pos,"0");
        x_final_pos = 0;
        y_final_pos = 0;

        output_print(fp1, comp_pos, recent_gcode, x_pos, y_pos, z_pos_cases[track], i_pos,
        j_pos, curr_tool, spindle_speed, &x_final_pos, &y_final_pos, x_comp_pos_cases[track], 
        y_comp_pos_cases[track], d_len2, prev_z_pos, previous_gcode, prev_i_pos, prev_j_pos,
        cutter_comp_direction);

        track = track + 1;
    }

    fclose(fp1);
    fp1 = fopen("../../test.txt", "r");
    if (fp1 == NULL) {
        perror("Error opening test data file");
        return;
    }

    char line1[BUFFERSIZE];
    char line2[BUFFERSIZE];
    int line_check = 0;
    while (fgets(line1, BUFFERSIZE, fp1) != NULL && fgets(line2, BUFFERSIZE, fp2) != NULL) {
        if (strcmp(line1,line2) == 0) {
            line_check = 1;
        } else {
            line_check = 0;
            break;
        }
    }
    fclose(fp1); // Close the test data file
    fclose(fp2);
    return line_check;
}

int intermediatePointsArc() {
    FILE *fp1 = fopen("../../test.txt", "w");
    FILE *fp2 = fopen("../../test_texts/output_print_w_arc_valid.txt","r");
    if (fp1 == NULL) {
        perror("Error opening test data file");
        return;
    }
    if (fp2 == NULL) {
        perror("Error opening test data file");
        return;
    }

    double x_comp_pos_cases[] = {0,1,2,0,-1.96825};
    double y_comp_pos_cases[] = {0,1,0,-2,.25};
    char *i_pos_cases[] = {"0","2","1","0","-1"};
    char *j_pos_cases[] = {".5","-1","0","-1","0"};
    char *prev_i_pos_cases[] = {"0","2","1","0","-1"};
    char *prev_j_pos_cases[] = {".5","-1","0","-1","0"};
    char *recent_gcode_cases[] = {"G2","G2","G3","G3","G3"};

    float d_len2 = 0.0;
    int comp_pos = 1;
    char x_pos[10] = "0", y_pos[10] = "0", z_pos[10] = "0";
    char cutter_comp_direction[10] = "n/a", i_pos[10] = "n/a", j_pos[10] = "n/a";
    char prev_z_pos[10] = "n/a";
    char prev_i_pos[10] = "n/a", prev_j_pos[10] = "n/a", curr_tool[10] = "1", spindle_speed[10] = "916";
    float x_final_pos = 0, y_final_pos = 0;
    char previous_gcode[10] = "n/a";
    int track = 0;

    while (track < 5) {
        strcpy(x_pos, "0");
        strcpy(y_pos, "0");
        strcpy(i_pos, "0");
        strcpy(j_pos, "0");
        strcpy(prev_i_pos, "0");
        strcpy(prev_j_pos, "0");
        x_final_pos = 0;
        y_final_pos = 0;

        output_print(fp1, comp_pos, recent_gcode_cases[track], x_pos, y_pos, z_pos, i_pos_cases[track],
        j_pos_cases[track], curr_tool, spindle_speed, &x_final_pos, &y_final_pos, x_comp_pos_cases[track], 
        y_comp_pos_cases[track], d_len2, prev_z_pos, previous_gcode, prev_i_pos_cases[track], prev_j_pos_cases[track],
        cutter_comp_direction);

        track = track + 1;
    }

    fclose(fp1);
    fp1 = fopen("../../test.txt", "r");
    if (fp1 == NULL) {
        perror("Error opening test data file");
        return;
    }

    char line1[BUFFERSIZE];
    char line2[BUFFERSIZE];
    int line_check = 0;
    while (fgets(line1, BUFFERSIZE, fp1) != NULL && fgets(line2, BUFFERSIZE, fp2) != NULL) {
        if (strcmp(line1,line2) == 0) {
            line_check = 1;
        } else {
            line_check = 0;
            break;
        }
    }
    fclose(fp1); // Close the test data file
    fclose(fp2);
    return line_check;
}
