#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#define BUFFERSIZE (256)
#include "output_print.c"
#include "error_printer.c"
#include "rest_token_creator.c"
#include "mcode_parse.c"
#include "gcode_parse.c"

char recent_gcode[BUFFERSIZE] = "n/a";
char previous_gcode[BUFFERSIZE] = "n/a";
char prev_x_pos[BUFFERSIZE] = "n/a";
char prev_y_pos[BUFFERSIZE] = "n/a";
char prev_z_pos[BUFFERSIZE] = "n/a";
char x_pos[BUFFERSIZE] = "0";
char y_pos[BUFFERSIZE] = "0";
char z_pos[BUFFERSIZE] = "0";
char prev_x_pos2[BUFFERSIZE] = "n/a";
char prev_y_pos2[BUFFERSIZE] = "n/a";
float x_comp_pos = NAN;
float y_comp_pos = NAN;
float x_final_pos = NAN;
float y_final_pos = NAN;
char i_pos[BUFFERSIZE] = "n/a";
char j_pos[BUFFERSIZE] = "n/a";
char prev_i_pos[BUFFERSIZE] = "n/a";
char prev_j_pos[BUFFERSIZE] = "n/a";
char k_pos[BUFFERSIZE] = "n/a";
char r_pos[BUFFERSIZE] = "n/a";
float d_len2 = NAN;
char feed_rate[BUFFERSIZE] = "n/a";
char curr_tool[BUFFERSIZE] = "n/a";
char spindle_speed[BUFFERSIZE] = "n/a";
int comp_count = 0;
char cutter_comp_direction[BUFFERSIZE] = "n/a";

int interpreter(int argc, char *argv[]);

int interpreter(int argc, char *argv[])
{
    FILE* ptr;
    char buffer[BUFFERSIZE];
    int count = 0;

    if ( argc != 2 ) /* argc should be 2 for correct execution */
    {
        printf("The only argument that should be given to the program is a file name.\n");
    }

    //change file path to your own local text test file//
    ptr = fopen(argv[1], "r");
    ////////////////////////////////////////////////////

    //output text filepath
    char *filename = "../output.txt";

    //output file for writing
    FILE *fp = fopen(filename, "w");
    //check for output file error
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
        return -1;
    }

    //check for file read error
    if (NULL == ptr) {
        printf("No file detected \n");
        fprintf(fp, "No file detected \n");
        return -1;
    }
    
    printf("File contents: \n");
    fprintf(fp, "File contents: \n");

    int g41_42_check = 0; //for use with diameter_seen

    while (fgets(buffer, BUFFERSIZE, ptr) != NULL) {
        //check to see if program starts with a "%"
        if (count == 0) {
            //if it doesn't throw an error
            if (strcmp(buffer, "%\n") && strcmp(buffer, "% \n") != 0) {
                printf("not proper file structure \n");
                fprintf(fp, "not proper file structure \n");     
                break;
            //if it does continue
            } else {
                count = count + 1;
            }
        } else {
            strcpy(previous_gcode, recent_gcode);
            count = count + 1;
            buffer[strcspn(buffer, "\n")] = 0;
            char *token = strtok(buffer, " ");
            
            //store old values if necessary
            if (strcmp(prev_x_pos2, prev_x_pos) != 0 || strcmp(prev_y_pos2, prev_y_pos) != 0) {
                strcpy(prev_x_pos2, prev_x_pos);
                strcpy(prev_y_pos2, prev_y_pos);
            }
            if (strcmp(prev_x_pos, x_pos) != 0 || strcmp(prev_y_pos, y_pos) != 0) {
                strcpy(prev_x_pos, x_pos);
                strcpy(prev_y_pos, y_pos);
            }
            if (strcmp(prev_z_pos, z_pos) != 0) {
                strcpy(prev_z_pos, z_pos);
            }
            if (strcmp(prev_i_pos, i_pos) != 0 || strcmp(prev_j_pos, j_pos) != 0) {
                strcpy(prev_i_pos, i_pos);
                strcpy(prev_j_pos, j_pos);
            } 

            //gets rid of ";"
            if (strstr(token, ";") != NULL && ((token[0] == 103 || token[0] == 71) 
            || (token[0] == 109 || token[0] == 77))) {
                char test[BUFFERSIZE] = "n/a";
                strcpy(test, token);
                test[strlen(token)-1] = '\0';
                token = test;
            }

            //check to see type of g command
            if ((token[0] == 103 || token[0] == 71) && (strcmp(token+1, "00") == 0 || strcmp(token+1, "0") == 0 ||
                strcmp(token+1, "01") == 0 || strcmp(token+1, "1") == 0 || strcmp(token+1, "02") == 0 ||
                strcmp(token+1, "2") == 0 || strcmp(token+1, "03") == 0 || strcmp(token+1, "3") == 0 ||
                strcmp(token+1, "40") == 0 || strcmp(token+1, "41") == 0 || strcmp(token+1, "42") == 0)){               

                char gcode[] = "G";
                strcat(gcode,token+1);
                token = strtok(NULL, " ");
                char *rest_token[7];
                int index = 0;
                int in_comment = 0; //tracker for if we are in a comment. 0 means not in a comment. 1 means in a comment.
                strcpy(recent_gcode,gcode); //track the most recent gcode passed in the variable "recent_gcode"

                //checks to see if there is a space between a coordinate and its value
                //for example = "X 36"
                //if there is a space it will concatenate those 2 token strings "X36"
                if (token != NULL && (strstr(token, "x") != NULL || strstr(token, "X") != NULL || strstr(token, "y") != NULL ||
                    strstr(token, "Y") != NULL || strstr(token, "z") != NULL || strstr(token, "Z") != NULL || 
                    strstr(token, "f") != NULL || strstr(token, "F") != NULL || strstr(token, "I") != NULL ||
                    strstr(token, "i") != NULL || strstr(token, "J") != NULL || strstr(token, "j") != NULL ||
                    strstr(token, "K") != NULL || strstr(token, "k") != NULL || 
                    strstr(token, "R") != NULL || strstr(token, "r") != NULL ||
                    strstr(token, "D") != NULL || strstr(token, "d") != NULL ) && strlen(token) == 1) {
                    char *concat = token;
                    token = strtok(NULL, " ");
                    strcat(concat, token);
                    token = concat;
                }

                char *mcode_token[7];
                int mcode_count = 0; //tracker for how many mcodes seen in a block
                int mcode_len = 0; //tracker for the number of mcode tokens

                //split rest of command string and put into "rest_token"
                while(token != NULL) {
                    //if we are in a comment ignore the string tokens
                    if (strstr(token, "(") != NULL && strstr(token, ")") == NULL) {
                        in_comment = 1;
                        token = strtok(NULL, " ");
                    //finds end of comment and changes in_comment to 0 to signal out of comment
                    } else if (strstr(token, ")") != NULL && strstr(token, "(") == NULL) {
                        in_comment = 0;
                        token = strtok(NULL, " ");
                    //checks to see if there is a space between a coordinate and its value
                    //for example = "X 36"
                    //if there is a space it will concatenate those 2 token strings "X36"
                    } else if (((strstr(token, "x") != NULL || strstr(token, "X") != NULL || strstr(token, "y") != NULL ||
                        strstr(token, "Y") != NULL || strstr(token, "z") != NULL || strstr(token, "Z") != NULL || 
                        strstr(token, "f") != NULL || strstr(token, "F") != NULL || strstr(token, "I") != NULL ||
                        strstr(token, "i") != NULL || strstr(token, "J") != NULL || strstr(token, "j") != NULL ||
                        strstr(token, "K") != NULL || strstr(token, "k") != NULL || 
                        strstr(token, "R") != NULL || strstr(token, "r") != NULL ||
                        strstr(token, "t") != NULL || strstr(token, "T") != NULL ||
                        strstr(token, "s") != NULL || strstr(token, "S") != NULL ||
                        strstr(token, "D") != NULL || strstr(token, "d") != NULL) && strlen(token) == 1) && in_comment == 0){
                        char *concat = token;
                        token = strtok(NULL, " "); 
                        strcat(concat, token);
                        token = concat;

                        //check if argument is spindle speed or tool change
                        if (strstr(token, "t") != NULL || strstr(token, "T") != NULL ||
                            strstr(token, "s") != NULL || strstr(token, "S") != NULL) {
                            mcode_token[mcode_len] = token;
                            mcode_len = mcode_len + 1;;
                        } else {
                            rest_token[index] = token;
                            index = index + 1;
                        }
                        token = strtok(NULL, " ");
                    //if not in comment then add values to rest_token
                    } else if (in_comment == 0 && strstr(token, "(") == NULL && strstr(token, ")") == NULL) {
                        //check if argument is an M code
                        if (strstr(token, "m") != NULL || strstr(token, "M") != NULL) {
                             //gets rid of ";"
                            if (strstr(token, ";") != NULL) {
                                char test[BUFFERSIZE] = "n/a";
                                strcpy(test, token);
                                test[strlen(token)-1] = '\0';
                                token = test;
                            }
                            mcode_token[mcode_len] = token;
                            mcode_count = mcode_count + 1;
                            mcode_len = mcode_len + 1;
                        //check if argument  is a tool change or spindle speed
                        } else if (strstr(token, "t") != NULL || strstr(token, "T") != NULL ||
                            strstr(token, "s") != NULL || strstr(token, "S") != NULL) {
                            mcode_token[mcode_len] = token;
                            mcode_len = mcode_len + 1;;
                        } else {
                            rest_token[index] = token;
                            index = index + 1;
                        }
                        token = strtok(NULL, " ");
                    } else {
                        token = strtok(NULL, " ");
                    }
                }

                //too many M codes on one line check
                if (mcode_count > 1) {
                    error_printer(fp, 2, count);
                    break;
                //M code argument given without corresponding M code
                } else if (mcode_count <= 0 && mcode_len > 0) {
                    error_printer(fp, 1, count);
                    break;
                }

                //Check to see if gcodes from same group are input on the same line
                int gcode_group_check = 0; //0 means gcodes are from different groups so no error
                for (int i = 0; i < index; i++) {
                    if (strstr(rest_token[i], "g") != NULL || strstr(rest_token[i], "G") != NULL) {
                        if (strstr("0010203", rest_token[i]+1) != NULL && strstr("0010203", gcode+1) != NULL) {
                            gcode_group_check = 1;
                        } else if ((strcmp("41",rest_token[i]+1) == 0 || strcmp("42",rest_token[i]+1) == 0 || 
                            strcmp("40",rest_token[i]+1) == 0) && (strcmp("41",gcode+1) == 0 || strcmp("42",gcode+1) == 0 || 
                            strcmp("40",gcode+1) == 0)) {
                            gcode_group_check = 1;
                        }
                    }
                }
                //1 means gcodes from same group so throw error
                if (gcode_group_check == 1) {
                    printf("g codes from same group input on line = %d\n\n", count);
                    fprintf(fp, "g codes from same group input on line = %d\n\n", count);
                    break;
                }

                //Check to see if comments were closed correctly
                if (in_comment == 1) {
                    error_printer(fp, 1, count); 
                    break;
                }

                //Clearing value for fresh run
                g41_42_check = 0;

                int len = index;
                index = 0;

                int check = gcode_parse(gcode, rest_token, len, g41_42_check, fp,
                &d_len2, &comp_count, x_pos, y_pos, z_pos,
                feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                r_pos, recent_gcode, prev_x_pos, prev_y_pos,
                &x_comp_pos, &y_comp_pos, prev_x_pos2, prev_y_pos2,
                previous_gcode);
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (check == 0) {
                    error_printer(fp, 1, count);
                    g41_42_check = 0;
                    break;
                } 

                int m_check = 1;
                //set spindle speed to 0 if m5 given
                if (mcode_len == 1 && (strcmp(mcode_token[0], "m05") == 0 || strcmp(mcode_token[0], "m5") == 0 ||
                    strcmp(mcode_token[0], "M05") == 0 || strcmp(mcode_token[0], "M5") == 0)) {
                    strcpy(spindle_speed, "0");
                //call mcode parse for tool change and spindle speed commands
                } else if (mcode_len == 2) {
                    char mcode[] = "M";
                    if (strstr(mcode_token[0],"m") != NULL || strstr(mcode_token[0],"M") != NULL) {
                        strcat(mcode,mcode_token[0]+1);
                        strcpy(mcode_token[0], mcode_token[1]);
                        mcode_len = mcode_len - 1;
                        m_check = mcode_parse(mcode, mcode_token, mcode_len, curr_tool, spindle_speed);
                    } else if (strstr(mcode_token[1],"m") != NULL || strstr(mcode_token[1],"M") != NULL) {
                        strcat(mcode,mcode_token[1]+1);
                        mcode_len = mcode_len - 1;
                        m_check = mcode_parse(mcode, mcode_token, mcode_len, curr_tool, spindle_speed);
                    }
                }
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (m_check == 0) {
                    error_printer(fp, 1, count);
                    break;
                }

                //Check for if valid g41/42 code recently passed in
                if (strcmp(recent_gcode+1, "42") == 0 || strcmp(recent_gcode+1,"41") == 0) {
                    g41_42_check = 1;
                }

                //Printed out the parsed data
                if ((strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0 ||
                    strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                    strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                    strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0) &&
                    (strcmp(rest_token[0],"g40") != 0 && strcmp(rest_token[0],"G40") != 0)){
                    output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                        prev_i_pos, prev_j_pos, cutter_comp_direction);
                } else if ((strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                    strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) || strcmp(rest_token[0],"g40") == 0 || strcmp(rest_token[0],"G40") == 0){
                    if (comp_count > 1) {
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode, 
                        prev_i_pos, prev_j_pos, cutter_comp_direction);
                    } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0 || strcmp(rest_token[0],"g40") == 0 || strcmp(rest_token[0],"G40") == 0) {
                        if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                            x_comp_pos = atof(prev_x_pos);
                        } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                            y_comp_pos = atof(prev_y_pos);
                        }
                        //command before g40
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                        prev_i_pos, prev_j_pos, cutter_comp_direction);
                        d_len2 = 0;
                        //g40 command
                        output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                        prev_i_pos, prev_j_pos, cutter_comp_direction);
                    }
                } else {
                    printf("\n");
                    fprintf(fp, "\n");
                }
                //stops if end program M code given
                if (mcode_len == 1 && (strstr("00102", mcode_token[0]+1) != NULL || strcmp(mcode_token[0]+1, "30") == 0)) {
                    printf("program ended at line = %d\n\n", count);
                    fprintf(fp, "program ended at line = %d\n\n", count);
                    break;
                }
            //check to see type of mcode
            } else if ((token[0] == 109 || token[0] == 77) && (strcmp(token+1, "00") == 0 || strcmp(token+1, "0") == 0 ||
                strcmp(token+1, "01") == 0 || strcmp(token+1, "1") == 0 || strcmp(token+1, "02") == 0 ||
                strcmp(token+1, "2") == 0 || strcmp(token+1, "03") == 0 || strcmp(token+1, "3") == 0 ||
                strcmp(token+1, "4") == 0 || strcmp(token+1, "04") == 0 || strcmp(token+1, "5") == 0 ||
                strcmp(token+1, "05") == 0 || strcmp(token+1, "06") == 0 || strcmp(token+1, "6") == 0 ||
                strcmp(token+1, "16") == 0 || strcmp(token+1, "30") == 0)){
                char mcode[] = "M";
                strcat(mcode,token+1);
                int seen_already = 0; 

                token = strtok(NULL, " ");
                char *rest_token[BUFFERSIZE];
                int index = 0;
                int in_comment = 0; //tracker for if we are in a comment. 0 means not in a comment. 1 means in a comment.

                //checks to see if there is a space between a argument and its value
                //for example = "t 36"
                //if there is a space it will concatenate those 2 token strings "t36"
                if (token != NULL && ((strstr(token, "t") != NULL || strstr(token, "T") != NULL || strstr(token, "s") != NULL ||
                    strstr(token, "S") != NULL) && strlen(token) == 1)) {
                    char *concat = token;
                    token = strtok(NULL, " ");
                    strcat(concat, token);
                    token = concat;
                }

                int arr[2];
                rest_token_creator(token, rest_token, index, in_comment, arr, 1);
                index = arr[0];
                in_comment = arr[1];

                //Check to see if comments were closed correctly
                if (in_comment == 1) { 
                    error_printer(fp, 3, count);
                    break;
                }

                //Check to make sure end program m codes and 
                //stop spindle speed m codes are only m codes on line
                if (index == 0) {
                    //if stop program mcode given then program is stopped
                    if (strstr("00102", mcode+1) != NULL || strcmp(mcode+1, "30") == 0) {
                        printf("program ended at line = %d\n\n", count);
                        fprintf(fp, "program ended at line = %d\n\n", count);
                        seen_already = 1;
                        break;
                    //if stop spindle command given set its speed to 0
                    } else if (strstr("05", mcode+1) != NULL) {
                        strcpy(spindle_speed, "0");
                        seen_already = 1;
                        //Printed out the parsed data
                        if (strcmp(recent_gcode, "n/a") == 0 ||
                            strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0 ||
                            strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                            strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                            strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                            &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                            prev_i_pos, prev_j_pos, cutter_comp_direction);
                        } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                            strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                            if (comp_count > 1) {
                                output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                                &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                                prev_i_pos, prev_j_pos, cutter_comp_direction);
                            } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                                if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                                    x_comp_pos = atof(prev_x_pos);
                                } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                                    y_comp_pos = atof(prev_y_pos);
                                }
                                //command before g40
                                output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                                &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                                prev_i_pos, prev_j_pos, cutter_comp_direction);
                                d_len2 = 0;
                                //g40 command
                                output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                                &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                                prev_i_pos, prev_j_pos, cutter_comp_direction);
                            }
                        } else {
                            printf("\n");
                            fprintf(fp, "\n");
                        }
                    }
                }

                int len = index;
                index = 0;
                int check = mcode_parse(mcode, rest_token, len, curr_tool, spindle_speed);
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (check == 0 && seen_already == 0) { 
                    error_printer(fp, 1, count);
                    break;
                }

                if (strcmp(mcode+1, "03") == 0 || strcmp(mcode+1,"04") == 0 || strcmp(mcode+1, "06") == 0 ||
                    strstr("346", mcode+1) != NULL || strcmp(mcode+1, "16") == 0) {
                    //Printed out the parsed data
                    if (strcmp(recent_gcode, "n/a") == 0 ||
                        strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0 ||
                        strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                        strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                        strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                        output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                        prev_i_pos, prev_j_pos, cutter_comp_direction);
                    } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                        strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                        if (comp_count > 1) {
                            output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                            &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode, prev_i_pos,
                            prev_j_pos, cutter_comp_direction);
                        } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                            if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                                x_comp_pos = atof(prev_x_pos);
                            } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                                y_comp_pos = atof(prev_y_pos);
                            }
                            //command before g40
                            output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                            &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode, 
                            prev_i_pos, prev_j_pos, cutter_comp_direction);
                            d_len2 = 0;
                            //g40 command
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                            &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                            prev_i_pos, prev_j_pos, cutter_comp_direction);
                        }
                    } else {
                        printf("\n");
                        fprintf(fp, "\n");
                    }
                }
            //for cases when tool change or spindle speed change is given before m code
            //for example: t3 m6;
            } else if ((strstr(token, "t") != NULL || strstr(token, "T") != NULL || strstr(token, "s") != NULL ||
                strstr(token, "S") != NULL) && strstr(token, "(") == NULL) {
                char test[1] = "n";
                test[0] = token[0];
                if (strstr(test, "t") != NULL || strstr(test, "T") != NULL || strstr(test, "s") != NULL ||
                    strstr(test, "S") != NULL) {

                    char *rest_token[BUFFERSIZE];
                    int index = 0;
                    int in_comment = 0; //tracker for if we are in a comment. 0 means not in a comment. 1 means in a comment.
                
                    //checks to see if there is a space between a coordinate and its value
                    //for example = "X 36"
                    //if there is a space it will concatenate those 2 token strings "X36"
                    if (strlen(token) == 1) {
                        char *concat = token;
                        token = strtok(NULL, " ");
                        strcat(concat, token);
                        token = concat;
                    }

                    int arr[2];
                    rest_token_creator(token, rest_token, index, in_comment, arr, 1);
                    index = arr[0];
                    in_comment = arr[1];

                    //Check to see if comments were closed correctly
                    if (in_comment == 1) {
                        error_printer(fp, 3, count);
                        break;
                    }

                    int len = index;
                    index = 0;
                    int check = 0;
                    char mcode[BUFFERSIZE] = "M";

                    if (len == 2 && (strstr(rest_token[1],"m") != NULL || strstr(rest_token[1],"M") != NULL)) {
                        if (strstr(rest_token[1], ";") != NULL) {
                            char test[BUFFERSIZE] = "n/a";
                            strcpy(test, rest_token[1]);
                            test[strlen(rest_token[1])-1] = '\0';
                            rest_token[1] = test;
                        }
                        strcat(mcode, rest_token[1]+1);
                        len = len - 1;
                        check = mcode_parse(mcode, rest_token, len, curr_tool, spindle_speed);
                    } else {
                        error_printer(fp, 1, count);
                        break;
                    }

                    //Check to see if commands followed proper syntax and form
                    //If not stop and print error
                    if (check == 0) {
                        error_printer(fp, 1, count);
                        break;
                    }

                    //Printed out the parsed data
                    if (strcmp(mcode+1, "03") == 0 || strcmp(mcode+1,"04") == 0 || strcmp(mcode+1, "06") == 0 ||
                    strstr("346", mcode+1) != NULL || strcmp(mcode+1, "16") == 0) {
                        if (strcmp(recent_gcode, "n/a") == 0 ||
                            strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0 ||
                            strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                            strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                            strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                            &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                            prev_i_pos, prev_j_pos, cutter_comp_direction);
                        } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                            strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                            if (comp_count > 1) {
                                output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                                &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                                prev_i_pos, prev_j_pos, cutter_comp_direction);
                            } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                                if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                                    x_comp_pos = atof(prev_x_pos);
                                } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                                    y_comp_pos = atof(prev_y_pos);
                                }
                                //command before g40
                                output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                                &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                                prev_i_pos, prev_j_pos, cutter_comp_direction);
                                d_len2 = 0;
                                //g40 command
                                output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                                &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                                prev_i_pos, prev_j_pos, cutter_comp_direction);
                            }
                        } else {
                            printf("\n");
                            fprintf(fp, "\n");
                        }
                    }

                }
            //stop interpreter if end of file token "%" seen
            } else if (strcmp(token, "%") == 0) {
                break;
            //checks to see if line is just a comment and that it is closed properly
            } else if (token[0] == 40) {
                int in_comment = 1; //tracker for if we are in a comment. 0 means not in a comment. 1 means in a comment.
                if (strstr(token, ")") != NULL && strstr(token, ";") != NULL) {
                    in_comment = 0;
                }
                token = strtok(NULL, " ");
                //check for closed parenthesis
                while (token != NULL) {
                    if (strstr(token, ")") != NULL) {
                        in_comment = 0;
                    }
                    token = strtok(NULL, " ");
                }
                //checks to see that comment closed properly
                if (in_comment == 1) {
                    error_printer(fp, 3, count);
                    break;
                }
                continue;
            //this handles the case when coordinates are given after a valid gcode
            //for example this should be allowed:
            //G01 x1 y4;
            //z6 x3;
            } else if (strcmp(recent_gcode, "n/a") != 0) {
                char *rest_token[7];
                int index = 0;
                int in_comment = 0; //tracker for if we are in a comment. 0 means not in a comment. 1 means in a comment.

                //checks to see if there is a space between a coordinate and its value
                //for example = "X 36"
                //if there is a space it will concatenate those 2 token strings "X36"
                if ((strstr(token, "x") != NULL || strstr(token, "X") != NULL || strstr(token, "y") != NULL ||
                    strstr(token, "Y") != NULL || strstr(token, "z") != NULL || strstr(token, "Z") != NULL || 
                    strstr(token, "f") != NULL || strstr(token, "F") != NULL || strstr(token, "I") != NULL ||
                    strstr(token, "i") != NULL || strstr(token, "J") != NULL || strstr(token, "j") != NULL ||
                    strstr(token, "K") != NULL || strstr(token, "k") != NULL || 
                    strstr(token, "R") != NULL || strstr(token, "r") != NULL) && strlen(token) == 1) {
                    char *concat = token;
                    token = strtok(NULL, " ");
                    strcat(concat, token);
                    token = concat;
                }

                int arr[2];
                rest_token_creator(token, rest_token, index, in_comment, arr, 2);
                index = arr[0];
                in_comment = arr[1];

                //Check to see if comments were closed correctly
                if (in_comment == 1) {
                    error_printer(fp, 3, count); 
                    break;
                }

                int len = index;
                index = 0;
                int check = gcode_parse(recent_gcode, rest_token, len, g41_42_check, fp,
                &d_len2, &comp_count, x_pos, y_pos, z_pos,
                feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                r_pos, recent_gcode, prev_x_pos, prev_y_pos,
                &x_comp_pos, &y_comp_pos, prev_x_pos2, prev_y_pos2,
                previous_gcode);
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (check == 0) {
                    error_printer(fp, 1, count);
                    break;
                }

                //Check for if valid g41/42 code recently passed in
                if (strcmp(recent_gcode+1, "42") == 0 || strcmp(recent_gcode+1,"41") == 0) {
                    g41_42_check = 1;
                } else {
                    g41_42_check = 0;
                }

                //Printed out the parsed data
                if (strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0 ||
                    strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                    strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                    strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                    output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                    &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                    prev_i_pos, prev_j_pos, cutter_comp_direction);
                } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                    strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                    if (comp_count > 0) {
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                        prev_i_pos, prev_j_pos, cutter_comp_direction);
                    } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                        if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                            x_comp_pos = atof(prev_x_pos);
                        } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                            y_comp_pos = atof(prev_y_pos);
                        }
                        //command before g40
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                        prev_i_pos, prev_j_pos, cutter_comp_direction);
                        d_len2 = 0;
                        //g40 command
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        &x_final_pos, &y_final_pos, x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                        prev_i_pos, prev_j_pos, cutter_comp_direction);
                    }
                } else {
                    printf("\n");
                    fprintf(fp, "\n");
                }
            } else {
                error_printer(fp, 1, count);
                break;  
            }
        }
    }
    fclose(ptr);
    fclose(fp);
    return 0;
}
