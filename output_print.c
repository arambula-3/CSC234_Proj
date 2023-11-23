#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#define BUFFERSIZE (256) 

//prints the outputs
void output_print(FILE *fp, int comp_pos, char *recent_gcode, char *x_pos, char *y_pos, char *z_pos, char *i_pos,
    char *j_pos, char *curr_tool, char *spindle_speed,
    float x_final_pos, float y_final_pos, float x_comp_pos, float y_comp_pos, float d_len2) {
    //Printed out the parsed data 
    if (strcmp(recent_gcode, "G40") == 0 || comp_pos == 1) {
        if (comp_pos == 1) {
            x_final_pos = x_comp_pos;
            y_final_pos = y_comp_pos;
        } else {
            x_final_pos = atof(x_pos);
            y_final_pos = atof(y_pos);
            if (strcmp(x_pos,"n/a") == 0) {
            x_final_pos = NAN;
            }
            if (strcmp(y_pos,"n/a") == 0) {
                y_final_pos = NAN;
            }
        }
    } else if (d_len2 == 0 || isnan(d_len2)) {
        x_final_pos = atof(x_pos);
        y_final_pos = atof(y_pos);
        if (strcmp(x_pos,"n/a") == 0) {
            x_final_pos = NAN;
        }
        if (strcmp(y_pos,"n/a") == 0) {
            y_final_pos = NAN;
        }
    } else {
        x_final_pos = x_comp_pos;
        y_final_pos = y_comp_pos;
    }
    printf("final x test= %.3f\n", x_final_pos);
    fprintf(fp, "final x test= %.3f\n", x_final_pos); 
    printf("final y test= %.3f\n", y_final_pos);  
    fprintf(fp, "final y test= %.3f\n", y_final_pos);
    if (strcmp(z_pos,"n/a") == 0) {
        printf("z= %.3f\n", NAN);
        fprintf(fp, "z= %.3f\n", NAN);
    } else {
        printf("z= %.3f\n", atof(z_pos));
        fprintf(fp, "z= %.3f\n", atof(z_pos));
    }
    printf("i= %s\n", i_pos);
    fprintf(fp, "i= %s\n", i_pos); 
    printf("j= %s\n", j_pos);
    fprintf(fp, "j= %s\n", j_pos); 
    printf("current tool= %s\n", curr_tool);
    fprintf(fp, "current tool= %s\n", curr_tool);
    printf("spindle speed= %s\n", spindle_speed);
    fprintf(fp, "spindle speed= %s\n", spindle_speed);
    ////final print format//////
    printf("| X Position | Y Position | Z Position | Current Tool | Spindle Speed |\n");
    fprintf(fp, "| X Position | Y Position | Z Position | Current Tool | Spindle Speed |\n");
    if (strcmp(z_pos,"n/a") == 0) {
        printf("| %.3f | %.3f | %.3f | %s | %s |\n", x_final_pos, y_final_pos, NAN, curr_tool, spindle_speed);
        fprintf(fp, "| %.3f | %.3f | %.3f | %s | %s |\n", x_final_pos, y_final_pos, NAN, curr_tool, spindle_speed);
    } else {
        printf("| %.3f | %.3f | %.3f | %s | %s |\n", x_final_pos, y_final_pos, atof(z_pos), curr_tool, spindle_speed);
        fprintf(fp, "| %.3f | %.3f | %.3f | %s | %s |\n", x_final_pos, y_final_pos, atof(z_pos), curr_tool, spindle_speed);
    }
    if (strcmp(recent_gcode, "G40") == 0) {
        d_len2 = 0;
    }
}