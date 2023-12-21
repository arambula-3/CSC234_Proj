#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#define BUFFERSIZE (256)
#include "num_check.c"
#include "cutter_compensation_validate.c"

// Assuming you have an enum to represent the planes
typedef enum {
    XY_PLANE = 0,
    XZ_PLANE,
    YZ_PLANE
} Plane;

// Variable to keep track of the current plane
Plane current_plane = XY_PLANE;

//gcode = Gcode passed in
//coords = array of X,Y,Z passed in
//len = number of arguments passed to coords
//diameter seen = if a valid g41/42 code has been passed before (1 = yes, 0 = no)
//function returns an int "check" 
//if check == 0
//then stop and print error, else continue
int gcode_parse(char *gcode, char *coords[], int len, int diameter_seen, FILE *fp,
    float *d_len2, int *comp_count, char *x_pos, char *y_pos, char *z_pos,
    char *feed_rate, char *cutter_comp_direction, char *i_pos, char *j_pos, char *k_pos,
    char *r_pos, char *recent_gcode, char *prev_x_pos, char *prev_y_pos,
    float *x_comp_pos, float *y_comp_pos, char *prev_x_pos2, char *prev_y_pos2,
    char *previous_gcode) {
    static char *ret[7];
    int check = 0;
    int valid_input = 1;
    //if statement for handling "G00" codes only
    if ((strcmp(gcode, "G00") == 0 || strcmp(gcode, "G0") == 0 || strcmp(gcode, "g0") == 0 || strcmp(gcode, "g00") == 0) &&
        (strcmp("g40",coords[0]) != 0 && strcmp("G40",coords[0]) != 0)) {
        for (int i = 0; i < len; i++) {
            int track;
            //Check to see that X coordinates are passed in
            if ((*coords[i] == 88) || (*coords[i] == 120)) {
                check = 1;
                track = 0;
            //Check to see that Y coordinates are passed in
            } else if ((*coords[i] == 89) || (*coords[i] == 121)) {
                check = 1;
                track = 1;
            //Check to see that Z coordinates are passed in 
            } else if ((*coords[i] == 90) || (*coords[i] == 122)) {
                check = 1;
                track = 2;
            } else if ((*coords[i] == 70) || (*coords[i] == 102)) {
                printf("Error: Feed rate is not allowed in G00 command.\n");
                fprintf(fp, "Error: Feed rate is not allowed in G00 command.\n");
                check = 0;
                valid_input = 0;
                break;
            } else if ((strcmp(coords[i], "g40") == 0 || strcmp(coords[i], "G40")) && i==0) {
                *d_len2 = 0;
                *comp_count = 0;
            } else {
                check = 0;
                break;
            }
            char *temp = coords[i];
            ret[i] = temp+1;

            //checks to see if token had a ";" attached to it
            //if it does it is removed
            if (strstr(ret[i], ";") != NULL) {
                char test[BUFFERSIZE] = "n/a";
                strcpy(test, ret[i]);
                test[strlen(ret[i])-1] = '\0';
                ret[i] = test;
            }

            //calls num_check function to see if token is a valid float value
            if (num_check(ret[i]) != 1) {
                check = 0;
                break;
            }
            if (valid_input){
                //change X's coordinates if necessary
                if (track == 0) {
                    strcpy(x_pos, ret[i]);
                //change Y's coordinates if necessary
                } else if (track == 1) {
                    strcpy(y_pos, ret[i]);
                //change Z's coordinates if necessary
                } else if (track == 2) {
                    strcpy(z_pos, ret[i]);
                } 
            }
        }
    //if statement for handling "G01" codes only
    } else if ((strcmp(gcode, "G01") == 0 || strcmp(gcode, "G1") == 0 || strcmp(gcode, "g1") == 0 || strcmp(gcode, "g01") == 0) &&
        (strcmp("g41",coords[0]) != 0 && strcmp("G41",coords[0]) != 0 && 
        strcmp("g42",coords[0]) != 0 && strcmp("G42",coords[0]) != 0 &&
        strcmp("g40",coords[0]) != 0 && strcmp("G40",coords[0]) != 0)) {
        int feedrate_given = 0;
        for (int i = 0; i < len; i++) {
            int track;
            //Check to see that X coordinates are passed in
            if ((*coords[i] == 88) || (*coords[i] == 120)) {
                check = 1;
                track = 0;
            //Check to see that Y coordinates are passed in
            } else if ((*coords[i] == 89) || (*coords[i] == 121)) {
                check = 1;
                track = 1;
            //Check to see that Z coordinates are passed in 
            } else if ((*coords[i] == 90) || (*coords[i] == 122)) {
                check = 1;
                track = 2;
            //Check to see that feedrate is passed in
            } else if ((*coords[i] == 70) || (*coords[i] == 102)) {
                check = 1;
                track = 3;
            } else {
                check = 0;
                break;
            }
            char *temp = coords[i];
            ret[i] = temp+1;

            //checks to see if token had a ";" attached to it
            //if it does it is removed
            if (strstr(ret[i], ";") != NULL) {
                char test[BUFFERSIZE] = "n/a";
                strcpy(test, ret[i]);
                test[strlen(ret[i])-1] = '\0';
                ret[i] = test;
            }

            //calls num_check function to see if token is a valid float value
            if (num_check(ret[i]) != 1) {
                check = 0;
                break;
            }

            //change X's coordinates if necessary
            if (track == 0) {
                strcpy(x_pos, ret[i]);
            //change Y's coordinates if necessary
            } else if (track == 1) {
                strcpy(y_pos, ret[i]);
            //change Z's coordinates if necessary
            } else if (track == 2) {
                strcpy(z_pos, ret[i]);
            //change F's coordinates if necessary
            } else if (track == 3){
                strcpy(feed_rate, ret[i]);
                feedrate_given = 1;
                float feed_rate_value = atof(feed_rate);
                //feed rate should be greater than or equal to 1
                if(feed_rate_value <1){
                    printf("Error: Feed rate must be greater than or equal to 1 \n");
                    fprintf(fp, "Error: Feed rate must be greater than or equal to 1 \n");
                    check = 0;
                    break;
                }
            }
        }

        if(!feedrate_given) {
            if(strcmp(feed_rate, "n/a") == 0){
                printf("Error: feed rate not given for g1 command \n");
                fprintf(fp, "Error: feed rate not given for g1 command \n");
                check = 0;
            }
        }
        if (*d_len2 > 0 && check != 0) {
            check = cutter_compensation_validate(cutter_comp_direction, *comp_count,
            x_pos, y_pos, d_len2, prev_x_pos, prev_y_pos,
            x_comp_pos, y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
            previous_gcode);
            *comp_count = *comp_count + 1;
            if (check == 0) {
                printf("Error: Too acute of angle corner cut\n");
                fprintf(fp, "Error: Too acute of angle corner cut\n");
            }
        }
    //if statement for handling "G02" and "G03" codes only
    } else if (strcmp(gcode, "G02") == 0 || strcmp(gcode, "G2") == 0 || strcmp(gcode, "g2") == 0 || strcmp(gcode, "g02") == 0 ||
        strcmp(gcode, "G03") == 0 || strcmp(gcode, "G3") == 0 || strcmp(gcode, "g3") == 0 || strcmp(gcode, "g03") == 0) {
        int feedrate_given = 0;
        int x_given = 0;
        int y_given = 0;
        int z_given = 0;
        int i_given = 0;
        int j_given = 0;
        int k_given = 0;
        for (int i = 0; i < len; i++) {
            int track;
            //Check to see that X coordinates are passed in
            if ((*coords[i] == 88) || (*coords[i] == 120)) {
                check = 1;
                track = 0;
            //Check to see that Y coordinates are passed in
            } else if ((*coords[i] == 89) || (*coords[i] == 121)) {
                check = 1;
                track = 1;
            //Check to see that Z coordinates are passed in 
            } else if ((*coords[i] == 90) || (*coords[i] == 122)) {
                check = 1;
                track = 2;
            //Check that feedrate is passed in
            } else if ((*coords[i] == 70) || (*coords[i] == 102)) {
                check = 1;
                track = 3;
            }  else if ((*coords[i] == 'I') || (*coords[i] == 'i')) {
                check = 1;
                track = 4;
            } else if ((*coords[i] == 'J') || (*coords[i] == 'j')) {
                check = 1;
                track = 5;
            } else if ((*coords[i] == 'K') || (*coords[i] == 'k')) {
                check = 1;
                track = 6;
            } else if ((*coords[i] == 'R') || (*coords[i] == 'r')) {
                check = 1;
                track = 7;
            } else {
                check = 0;
                break;
            }
            char *temp = coords[i];
            ret[i] = temp+1;

            //checks to see if token had a ";" attached to it
            //if it does it is removed
            if (strstr(ret[i], ";") != NULL) {
                char test[BUFFERSIZE] = "n/a";
                strcpy(test, ret[i]);
                test[strlen(ret[i])-1] = '\0';
                ret[i] = test;
            }

            //calls num_check function to see if token is a valid float value
            if (num_check(ret[i]) != 1) {
                check = 0;
                break;
            }

            //change X's coordinates if necessary
            if (track == 0) {
                strcpy(x_pos, ret[i]);
                x_given = 1;
            //change Y's coordinates if necessary
            } else if (track == 1) {
                y_given = 1;
                strcpy(y_pos, ret[i]);
            //change Z's coordinates if necessary
            } else if (track == 2) {
                z_given = 1;
                strcpy(z_pos, ret[i]);
            //change F's coordinates if necessary
            } else if (track == 3){
                strcpy(feed_rate, ret[i]);
                feedrate_given = 1;
                float feed_rate_value = atof(feed_rate);
                //feed rate should be greater than or equal to 1
                if(feed_rate_value <1){
                    printf("Error: Feed rate must be greater than or equal to 1 \n");
                    fprintf(fp, "Error: Feed rate must be greater than or equal to 1 \n");
                    check = 0;
                    break;
                }
            } else if (track == 4) {
                i_given = 1;
                strcpy(i_pos, ret[i]);
            } else if (track == 5) {
                j_given = 1;
                strcpy(j_pos, ret[i]);
            } else if (track == 6) {
                k_given = 1;
                strcpy(k_pos, ret[i]);
                float k_value = atof(k_pos);
                if(k_value<=0){
                    printf("k value can't be less than or equal to 0 \n");
                    fprintf(fp, "k value can't be less than or equal to 0 \n");
                }
            } else if (track == 7) {
                strcpy(r_pos, ret[i]);
                float r_value = atof(r_pos);
                if(r_value == 0){
                    printf("r value can't be less than or equal to 0 \n");
                    fprintf(fp, "r value can't be less than or equal to 0 \n");
                }
            }

            //make I = 0 if J is given and I not given
            //make J = 0 if I is given and J not given
            if (i_given != 1 && j_given == 1) {
                strcpy(i_pos, "0");
            } else if (j_given != 1 && i_given == 1) {
                strcpy(j_pos, "0");
            }
        }

        //check if I and J value given is valid
        float temp_i = atof(i_pos);
        float temp_j = atof(j_pos);
        float radius_check = 0;
        float x_arc_center = atof(prev_x_pos) + temp_i;
        float y_arc_center = atof(prev_y_pos) + temp_j;
        float a = atof(x_pos) - x_arc_center;
        float b = atof(y_pos) - y_arc_center;
        float temp_radius = sqrt(a * a + b * b);
        if (temp_i == 0) {
            radius_check = fabs(temp_j);
        } else if (temp_j == 0) {
            radius_check = fabs(temp_i);
        } else {
            radius_check = sqrt(temp_i * temp_i + temp_j * temp_j);
        }
        if (temp_radius != radius_check) {
            printf("Error: I and J value do not create a valid arc to X and Y \n");
            fprintf(fp, "Error: I and J value do not create a valid arc to X and Y \n");
            check = 0;
        }

        // chooses current_plane based on G17,18,19 
        switch (current_plane) {
            case XY_PLANE:
                // Validate for XY plane
                if ((!x_given || !y_given) && check != 0) {
                    if(strcmp(x_pos, "n/a") == 0){
                    printf("Error: X position not given for g2 or g3 command \n");
                    fprintf(fp, "Error: X position not given for g2 or g3 command \n");
                    check = 0;
                }
                if(strcmp(y_pos, "n/a") == 0 && check != 0){
                    printf("Error: Y position not given for g2 or g3 command \n");
                    fprintf(fp, "Error: Y position not given for g2 or g3 command \n");
                    check = 0;
                }
                else{
                    if(strcmp(i_pos, "n/a")== 0 && strcmp(j_pos, "n/a")== 0 && i_given == 0 && j_given == 0 && check != 0){
                    printf("Error:  i or j not given for g2 or g3 command \n");
                    fprintf(fp, "Error:  i or j not given for g2 or g3 command \n");
                    check = 0;
                    }
                }
                }
                else{
                if(i_given == 0 && j_given == 0 && check != 0){
                    if(strcmp(i_pos, "n/a")== 0 && strcmp(j_pos, "n/a")== 0){
                    printf("Error:  i or j not given for g2 or g3 command \n");
                    fprintf(fp, "Error:  i or j not given for g2 or g3 command \n");
                    check = 0;
                    }
                }
            }

                break;
            case XZ_PLANE:
                // Validate for XZ plane
                if (!x_given || !z_given) {
                    if(strcmp(x_pos, "n/a") == 0){
                    printf("Error: X position not given for g2 or g3 command \n");
                    fprintf(fp, "Error: X position not given for g2 or g3 command \n");
                }
                if(strcmp(z_pos, "n/a") == 0){
                    printf("Error: Z position not given for g2 or g3 command \n");
                    fprintf(fp, "Error: Z position not given for g2 or g3 command \n");
                }
                else{
                    if(strcmp(i_pos, "n/a")== 0 && strcmp(k_pos, "n/a")== 0 && i_given == 0 && k_given == 0){
                    printf("Error:  i or k not given for g2 or g3 command \n");
                    fprintf(fp, "Error:  i or k not given for g2 or g3 command \n");
                    }
                }
                }
                else{
                if(i_given == 0 && k_given == 0){
                    if(strcmp(i_pos, "n/a")== 0 && strcmp(k_pos, "n/a")== 0){
                    printf("Error:  i or k not given for g2 or g3 command \n");
                    fprintf(fp, "Error:  i or k not given for g2 or g3 command \n");
                    }
                }
            }

                break;
            case YZ_PLANE:
                // Validate for YZ plane
                if (!z_given || !y_given) {
                    if(strcmp(z_pos, "n/a") == 0){
                    printf("Error: Z position not given for g2 or g3 command \n");
                    fprintf(fp,"Error: Z position not given for g2 or g3 command \n");
                }
                if(strcmp(y_pos, "n/a") == 0){
                    printf("Error: Y position not given for g2 or g3 command \n");
                    fprintf(fp, "Error: Y position not given for g2 or g3 command \n");
                }
                else{
                    if(strcmp(j_pos, "n/a")== 0 && strcmp(k_pos, "n/a")== 0 && z_given == 0 && j_given == 0){
                    printf("Error:  k or j not given for g2 or g3 command \n");
                    fprintf(fp, "Error:  k or j not given for g2 or g3 command \n");
                    }
                }
                }
                else{
                if(k_given == 0 && j_given == 0){
                    if(strcmp(i_pos, "n/a")== 0 && strcmp(j_pos, "n/a")== 0){
                    printf("Error:  k or j not given for g2 or g3 command \n");
                    fprintf(fp, "Error:  k or j not given for g2 or g3 command \n");
                    }
                }
            }


            break;
        }

        if(!feedrate_given) {
            if(strcmp(feed_rate, "n/a") == 0){
                printf("Error: feed rate not given for g2 or g3 command \n");
                fprintf(fp, "Error: feed rate not given for g2 or g3 command \n");
                check = 0;
            }
        }

        //checks both I and J cannot be zero
        if (i_given && atof(i_pos) == 0 && j_given && atof(j_pos) == 0) {
            printf("Error: I and J cannot both be zero for G02/G03 commands\n");
            fprintf(fp, "Error: I and J cannot both be zero for G02/G03 commands\n");
            check = 0;
        }
    
        //checks if both R and I/J values given
        if ((i_given || j_given) && (strcmp(r_pos, "n/a") != 0)) {
            printf("Error: Cannot use both I/J and R for G02/G03 commands\n");
            fprintf(fp, "Error: Cannot use both I/J and R for G02/G03 commands\n");
            check = 0;
        }

        if (*d_len2 > 0 && check != 0) {
            check = cutter_compensation_validate(cutter_comp_direction, *comp_count,
            x_pos, y_pos, d_len2, prev_x_pos, prev_y_pos,
            x_comp_pos, y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
            previous_gcode);
            *comp_count = *comp_count + 1;
            if (check == 0) {
                printf("Error: Too acute of angle corner cut\n");
                fprintf(fp, "Error: Too acute of angle corner cut\n");
            }
        }
    //If statement for handling g40 codes only
    } else if ((strcmp(gcode, "G40") == 0 || strcmp(gcode, "g40") == 0) ||
        ((strcmp(gcode+1,"01") == 0 || strcmp(gcode+1,"1") == 0 || strcmp(gcode+1, "00") == 0 || strcmp(gcode+1,"0") == 0) && 
        (strcmp(coords[0], "g40") == 0 || strcmp(coords[0], "G40") == 0))) {
        for (int i = 0; i < len; i++) {
            int track;
            *comp_count = -1;
            //Check to see that X coordinates are passed in
            if ((*coords[i] == 88) || (*coords[i] == 120)) {
                check = 1;
                track = 0;
            //Check to see that Y coordinates are passed in
            } else if ((*coords[i] == 89) || (*coords[i] == 121)) {
                check = 1;
                track = 1;
            } else if ((strcmp(coords[i], "g0") == 0 || strcmp(coords[i], "G0") || 
                strcmp(coords[i], "g00") == 0 || strcmp(coords[i], "G00") ||
                strcmp(coords[i], "g1") == 0 || strcmp(coords[i], "G1") || 
                strcmp(coords[i], "g01") == 0 || strcmp(coords[i], "G01") ||
                strcmp(coords[i], "g40") == 0 || strcmp(coords[i], "G40")) && i==0) {
                continue;
            } else {
                check = 0;
                break;
            }
            char *temp = coords[i];
            ret[i] = temp+1;

            //checks to see if token had a ";" attached to it
            //if it does it is removed
            if (strstr(ret[i], ";") != NULL) {
                char test[BUFFERSIZE] = "n/a";
                strcpy(test, ret[i]);
                test[strlen(ret[i])-1] = '\0';
                ret[i] = test;
            }

            //calls num_check function to see if token is a valid float value
            if (num_check(ret[i]) != 1) {
                check = 0;
                break;
            }
            if (valid_input){
                //change X's coordinates if necessary
                if (track == 0) {
                    strcpy(x_pos, ret[i]);
                //change Y's coordinates if necessary
                } else if (track == 1) {
                    strcpy(y_pos, ret[i]);
                } 
            }
        }
        if (check != 0) {
            strcpy(recent_gcode, "G40");
            check = cutter_compensation_validate(cutter_comp_direction, *comp_count,
            x_pos, y_pos, d_len2, prev_x_pos, prev_y_pos,
            x_comp_pos, y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
            previous_gcode);
            *comp_count = 0;
            if (check == 0) {
                printf("Error: Too acute of angle corner cut\n");
                fprintf(fp, "Error: Too acute of angle corner cut\n");
            }
        }
    //If statement for handling g41/42 codes only
    } else if ((strcmp(gcode, "G41") == 0 || strcmp(gcode, "G42") == 0 || strcmp(gcode, "g41") == 0 || strcmp(gcode, "g42") == 0) ||
        ((strcmp(gcode+1,"01") == 0 || strcmp(gcode+1,"1") == 0) && 
        (strcmp(coords[0], "g41") == 0 || strcmp(coords[0], "G41") == 0 || strcmp(coords[0], "g42") == 0 || strcmp(coords[0], "G42") == 0))){
        int diameter_given = 0;
        int x_or_y_given = 0;
        if (strcmp(gcode+1,"41") == 0) {
            strcpy(cutter_comp_direction, "left"); 
        } else if (strcmp(gcode+1,"42") == 0) {
            strcpy(cutter_comp_direction,"right");
        }
        for (int i = 0; i < len; i++) {
            int track;
            char *temp = coords[i];
            //Check to see that X coordinates are passed in
            if ((*coords[i] == 88) || (*coords[i] == 120)) {
                check = 1;
                track = 0;
                x_or_y_given = 1;
            //Check to see that Y coordinates are passed in
            } else if ((*coords[i] == 89) || (*coords[i] == 121)) {
                check = 1;
                track = 1;
                x_or_y_given = 1;
            //Check to see that Z coordinates are not passed in 
            } else if ((*coords[i] == 90) || (*coords[i] == 122)) {
                check = 0;
                break;
            //Check to see that feed rate is passed in
            } else if ((*coords[i] == 70) || (*coords[i] == 102)) {
                check = 1;
                track = 2;
             //Check to see that diameter value is passed in 
            } else if ((*coords[i] == 68) || (*coords[i] == 100)) {
                check = 1;
                track = 3;
                diameter_given = 1;
            } else if (strstr(temp, "01") != NULL || strstr(temp, "1") != NULL || 
                strstr(temp,"42") != NULL || strstr(temp,"41") != NULL) {
                if (strcmp(temp+1,"41") == 0) {
                    strcpy(cutter_comp_direction, "left");
                    strcpy(recent_gcode, "G41"); 
                } else if (strcmp(temp+1,"42") == 0) {
                    strcpy(cutter_comp_direction,"right");
                    strcpy(recent_gcode,"G42");
                }
                continue;
            } else {
                check = 0;
                break;
            }
            ret[i] = temp+1;

            //checks to see if token had a ";" attached to it
            //if it does it is removed
            if (strstr(ret[i], ";") != NULL) {
                char test[BUFFERSIZE] = "n/a";
                strcpy(test, ret[i]);
                test[strlen(ret[i])-1] = '\0';
                ret[i] = test;
            }

            //calls num_check function to see if token is a valid float value
            if (num_check(ret[i]) != 1) {
                check = 0;
                break;
            }

            if (check != 0 && track == 3) {
                float num;
                sscanf(ret[i], "%f", &num);
                if (signbit(num) != 0 || num > 200) {
                    check = 0;
                    break;
                }
            }

            //change X's coordinates if necessary
            if (track == 0) {
                strcpy(x_pos, ret[i]);
            //change Y's coordinates if necessary
            } else if (track == 1) {
                strcpy(y_pos, ret[i]);
            //change feedrate if necessary
            } else if (track == 2) {
                strcpy(feed_rate, ret[i]);
            //change diameter value if necessary
            } else if (track == 3){
                *d_len2 = atof(ret[i]);
            }
        }
        if ((diameter_given != 1 || x_or_y_given != 1) && diameter_seen == 0) {
            check = 0;
        }
        if (check != 0) {
            check = cutter_compensation_validate(cutter_comp_direction, *comp_count,
            x_pos, y_pos, d_len2, prev_x_pos, prev_y_pos,
            x_comp_pos, y_comp_pos, recent_gcode, prev_x_pos2, prev_y_pos2,
            previous_gcode);
            *comp_count = *comp_count + 1;
            if (check == 0) {
                printf("Error: Too acute of angle corner cut\n");
                fprintf(fp, "Error: Too acute of angle corner cut\n");
            }
        }
    }
    return check;
}
