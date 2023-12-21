#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#define BUFFERSIZE (256)
#include "output_print.h"
#include "error_printer.h"
#include "rest_token_creator.h"
#include "mcode_parse.h"
#include "gcode_parse.h"

//int gcode_parse(char *gcode, char *coords[], int len, int diameter_seen, FILE *fp);

char recent_gcode[BUFFERSIZE] = "n/a";
char previous_gcode[BUFFERSIZE] = "n/a";
char prev_x_pos[BUFFERSIZE] = "n/a";
char prev_y_pos[BUFFERSIZE] = "n/a";
char x_pos[BUFFERSIZE] = "n/a";
float x_pos2 = NAN;
char y_pos[BUFFERSIZE] = "n/a";
char z_pos[BUFFERSIZE] = "n/a";
char prev_x_pos2[BUFFERSIZE] = "n/a";
char prev_y_pos2[BUFFERSIZE] = "n/a";
float x_comp_pos = NAN;
float y_comp_pos = NAN;
float x_final_pos = NAN;
float y_final_pos = NAN;
char i_pos[BUFFERSIZE] = "n/a";
char j_pos[BUFFERSIZE] = "n/a";
char k_pos[BUFFERSIZE] = "n/a";
char r_pos[BUFFERSIZE] = "n/a";
float d_len2 = NAN;
char feed_rate[BUFFERSIZE] = "n/a";
char curr_tool[BUFFERSIZE] = "n/a";
char spindle_speed[BUFFERSIZE] = "n/a";
int comp_count = 0;
char cutter_comp_direction[BUFFERSIZE] = "n/a";

int main(void)
{
    FILE* ptr;
    char buffer[BUFFERSIZE];
    int count = 0;

    //Will need to change file path to your own local temporary text test file
    ptr = fopen("../temp_test6.txt", "r");

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
            
            if (strcmp(prev_x_pos2, prev_x_pos) != 0 || strcmp(prev_y_pos2, prev_y_pos)) {
                strcpy(prev_x_pos2, prev_x_pos);
                strcpy(prev_y_pos2, prev_y_pos);
            }
            if (strcmp(prev_x_pos, x_pos) != 0 || strcmp(prev_y_pos, y_pos)) {
                strcpy(prev_x_pos, x_pos);
                strcpy(prev_y_pos, y_pos);
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
                int mcode_count = 0;
                int mcode_len = 0;

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
                    //break;
                //M code argument given without corresponding M code
                } else if (mcode_count <= 0 && mcode_len > 0) {
                    error_printer(fp, 1, count);
                    //break;
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
                    //break;
                }

                //Check to see if comments were closed correctly
                if (in_comment == 1) {
                    error_printer(fp, 1, count); 
                    //break;
                }

                //Clearing value for fresh run
                g41_42_check = 0;

                int len = index;
                index = 0;

                int check = gcode_parse(gcode, rest_token, len, g41_42_check, fp,
                d_len2, comp_count, x_pos, y_pos, z_pos, x_pos2,
                feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                r_pos, recent_gcode, prev_x_pos, prev_y_pos,
                x_comp_pos, y_comp_pos, prev_x_pos2, prev_y_pos2,
                previous_gcode);
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (check == 0) {
                    error_printer(fp, 1, count);
                    g41_42_check = 0;
                    //break;
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
                    //break;
                }

                //Check for if valid g41/42 code recently passed in
                if (strcmp(recent_gcode+1, "42") == 0 || strcmp(recent_gcode+1,"41") == 0) {
                    g41_42_check = 1;
                }

                //Printed out the parsed data
                ///////////////////////
                //FILE *fp, int comp_pos, char *recent_gcode, char *x_pos, char *y_pos, char *z_pos, char *i_pos,
    //char *j_pos, char *curr_tool, char *spindle_speed,
    //float x_final_pos, float y_final_pos, float x_comp_pos, float y_comp_pos, float d_len2)
                //output_print(fp);
                if (strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0) {
                    //output_print(fp, 0);
                    output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                    printf("final x pos= %s\n", x_pos);
                    fprintf(fp, "final x pos= %s\n", x_pos);
                    printf("final y pos= %s\n\n", y_pos);
                    fprintf(fp, "final y pos= %s\n\n", y_pos);
                } else if ((strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                    strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                    strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0) &&
                    (strcmp(rest_token[0],"g40") != 0 && strcmp(rest_token[0],"G40") != 0)){
                    //output_print(fp, 0);
                    output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                    printf("final x pos= %s\n", x_pos);
                    fprintf(fp, "final x pos= %s\n", x_pos);
                    printf("final y pos= %s\n", y_pos);
                    fprintf(fp, "final y pos= %s\n", y_pos);
                    printf("feed rate= %s\n\n", feed_rate);
                    fprintf(fp, "feed rate= %s\n\n", feed_rate);
                } else if ((strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                    strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) || strcmp(rest_token[0],"g40") == 0 || strcmp(rest_token[0],"G40") == 0){
                    if (comp_count > 1) {
                        //output_print(fp, 1);
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                        printf("final x pos= %f\n", x_comp_pos);
                        fprintf(fp, "final x pos= %f\n", x_comp_pos);
                        printf("final y pos= %f\n", y_comp_pos);
                        fprintf(fp, "final y pos= %f\n", y_comp_pos);
                        ///////////////////////////////////////
                        printf("diameter= %.3f\n\n", d_len2);
                        fprintf(fp, "diameter= %.3f\n\n", d_len2);
                    } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0 || strcmp(rest_token[0],"g40") == 0 || strcmp(rest_token[0],"G40") == 0) {
                        //printf("here\n");
                        if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                            x_comp_pos = atof(prev_x_pos);
                        } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                            y_comp_pos = atof(prev_y_pos);
                        }
                        //output_print(fp, 1);
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                        printf("final x pos= %f\n", x_comp_pos);
                        fprintf(fp, "final x pos= %f\n", x_comp_pos);
                        printf("final y pos= %f\n", y_comp_pos);
                        fprintf(fp, "final y pos= %f\n", y_comp_pos);
                        ///////////////////////////////////////
                        printf("diameter= %.3f\n\n", d_len2);
                        fprintf(fp, "diameter= %.3f\n\n", d_len2);

                        //output_print(fp, 0);
                        output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                        printf("final x pos= %s\n", x_pos);
                        fprintf(fp, "final x pos= %s\n", x_pos);
                        printf("final y pos= %s\n\n", y_pos);
                        fprintf(fp, "final y pos= %s\n\n", y_pos);
                    }
                } else {
                    printf("\n");
                    fprintf(fp, "\n");
                }
                //stops if end program M code given
                if (mcode_len == 1 && (strstr("00102", mcode_token[0]+1) != NULL || strcmp(mcode_token[0]+1, "30") == 0)) {
                    printf("program ended at line = %d\n\n", count);
                    fprintf(fp, "program ended at line = %d\n\n", count);
                    //break;
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
                    //break;
                }

                //Check to make sure end program m codes and 
                //stop spindle speed m codes are only m codes on line
                if (index == 0) {
                    //if stop program mcode given then program is stopped
                    if (strstr("00102", mcode+1) != NULL || strcmp(mcode+1, "30") == 0) {
                        printf("program ended at line = %d\n\n", count);
                        fprintf(fp, "program ended at line = %d\n\n", count);
                        seen_already = 1;
                        //break;
                    //if stop spindle command given set its speed to 0
                    } else if (strstr("05", mcode+1) != NULL) {
                        strcpy(spindle_speed, "0");
                        seen_already = 1;
                        //Printed out the parsed data
                        //output_print(fp);
                        if (strcmp(recent_gcode, "n/a") == 0) {
                            //output_print(fp, 0);
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                            printf("\n");
                            fprintf(fp, "\n");
                        } else if (strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0) {
                            //output_print(fp, 0);
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                            printf("final x pos= %s\n", x_pos);
                            fprintf(fp, "final x pos= %s\n", x_pos);
                            printf("final y pos= %s\n\n", y_pos);
                            fprintf(fp, "final y pos= %s\n\n", y_pos);
                        } else if (strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                            strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                            strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                            //output_print(fp, 0);
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                            printf("final x pos= %s\n", x_pos);
                            fprintf(fp, "final x pos= %s\n", x_pos);
                            printf("final y pos= %s\n", y_pos);
                            fprintf(fp, "final y pos= %s\n", y_pos);
                            printf("feed rate= %s\n\n", feed_rate);
                            fprintf(fp, "feed rate= %s\n\n", feed_rate);
                        } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                            strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                            ///////////////////////////////////
                            //printf("diameter= %.3f\n\n", d_len2);
                            //fprintf(fp, "diameter= %.3f\n\n", d_len2);
                            if (comp_count > 1) {
                                //output_print(fp, 1);
                                output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                                printf("final x pos= %f\n", x_comp_pos);
                                fprintf(fp, "final x pos= %f\n", x_comp_pos);
                                printf("final y pos= %f\n", y_comp_pos);
                                fprintf(fp, "final y pos= %f\n", y_comp_pos);
                                ///////////////////////////////////////
                                printf("diameter= %.3f\n\n", d_len2);
                                fprintf(fp, "diameter= %.3f\n\n", d_len2);
                            } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                                if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                                    x_comp_pos = atof(prev_x_pos);
                                } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                                    y_comp_pos = atof(prev_y_pos);
                                }
                                //output_print(fp, 1);
                                output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                                printf("final x pos= %f\n", x_comp_pos);
                                fprintf(fp, "final x pos= %f\n", x_comp_pos);
                                printf("final y pos= %f\n", y_comp_pos);
                                fprintf(fp, "final y pos= %f\n", y_comp_pos);
                                ///////////////////////////////////////
                                printf("diameter= %.3f\n\n", d_len2);
                                fprintf(fp, "diameter= %.3f\n\n", d_len2);

                                //output_print(fp, 0);
                                output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                                printf("final x pos= %s\n", x_pos);
                                fprintf(fp, "final x pos= %s\n", x_pos);
                                printf("final y pos= %s\n\n", y_pos);
                                fprintf(fp, "final y pos= %s\n\n", y_pos);
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
                    //break;
                }

                if (strcmp(mcode+1, "03") == 0 || strcmp(mcode+1,"04") == 0 || strcmp(mcode+1, "06") == 0 ||
                    strstr("346", mcode+1) != NULL || strcmp(mcode+1, "16") == 0) {
                    //Printed out the parsed data
                    if (strcmp(recent_gcode, "n/a") == 0) {
                        //output_print(fp, 0);
                        output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                        printf("\n");
                        fprintf(fp, "\n");
                    } else if (strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0) {
                        //output_print(fp, 0);
                        output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                        printf("final x pos= %s\n", x_pos);
                        fprintf(fp, "final x pos= %s\n", x_pos);
                        printf("final y pos= %s\n\n", y_pos);
                        fprintf(fp, "final y pos= %s\n\n", y_pos);
                    } else if (strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                        strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                        strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                        //output_print(fp, 0);
                        output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                        printf("final x pos= %s\n", x_pos);
                        fprintf(fp, "final x pos= %s\n", x_pos);
                        printf("final y pos= %s\n\n", y_pos);
                        fprintf(fp, "final y pos= %s\n\n", y_pos);
                        printf("feed rate= %s\n\n", feed_rate);
                        fprintf(fp, "feed rate= %s\n\n", feed_rate);
                    } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                        strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                        /////////////////////////////////////////
                        //printf("diameter= %.3f\n\n", d_len2);
                        //fprintf(fp, "diameter= %.3f\n\n", d_len2);
                        if (comp_count > 1) {
                            //output_print(fp, 1);
                            output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                            printf("final x pos= %f\n", x_comp_pos);
                            fprintf(fp, "final x pos= %f\n", x_comp_pos);
                            printf("final y pos= %f\n", y_comp_pos);
                            fprintf(fp, "final y pos= %f\n", y_comp_pos);
                            ///////////////////////////////////////
                            printf("diameter= %.3f\n\n", d_len2);
                            fprintf(fp, "diameter= %.3f\n\n", d_len2);
                        } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                            //printf("here\n");
                            if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                                x_comp_pos = atof(prev_x_pos);
                            } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                                y_comp_pos = atof(prev_y_pos);
                            }
                            //output_print(fp, 1);
                            output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                            printf("final x pos= %f\n", x_comp_pos);
                            fprintf(fp, "final x pos= %f\n", x_comp_pos);
                            printf("final y pos= %f\n", y_comp_pos);
                            fprintf(fp, "final y pos= %f\n", y_comp_pos);
                            ///////////////////////////////////////
                            printf("diameter= %.3f\n\n", d_len2);
                            fprintf(fp, "diameter= %.3f\n\n", d_len2);

                            //output_print(fp, 0);
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                            printf("final x pos= %s\n", x_pos);
                            fprintf(fp, "final x pos= %s\n", x_pos);
                            printf("final y pos= %s\n\n", y_pos);
                            fprintf(fp, "final y pos= %s\n\n", y_pos);
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
                        //break;
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
                        //break;
                    }

                    //Check to see if commands followed proper syntax and form
                    //If not stop and print error
                    if (check == 0) {
                        error_printer(fp, 1, count);
                        //break;
                    }

                    if (strcmp(mcode+1, "03") == 0 || strcmp(mcode+1,"04") == 0 || strcmp(mcode+1, "06") == 0 ||
                    strstr("346", mcode+1) != NULL || strcmp(mcode+1, "16") == 0) {
                        //Printed out the parsed data
                        //output_print(fp);
                        if (strcmp(recent_gcode, "n/a") == 0) {
                            //output_print(fp, 0);
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                            printf("\n");
                            fprintf(fp, "\n");
                        } else if (strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0) {
                            //output_print(fp, 0);
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                            printf("final x pos= %s\n", x_pos);
                            fprintf(fp, "final x pos= %s\n", x_pos);
                            printf("final y pos= %s\n\n", y_pos);
                            fprintf(fp, "final y pos= %s\n\n", y_pos);
                        } else if (strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                            strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                            strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                            //output_print(fp, 0);
                            output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                            printf("final x pos= %s\n", x_pos);
                            fprintf(fp, "final x pos= %s\n", x_pos);
                            printf("final y pos= %s\n\n", y_pos);
                            fprintf(fp, "final y pos= %s\n\n", y_pos);
                            printf("feed rate= %s\n\n", feed_rate);
                            fprintf(fp, "feed rate= %s\n\n", feed_rate);
                        } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                            strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                            //////////////////////////////////////////
                            //printf("diameter= %.3f\n\n", d_len2);
                            //fprintf(fp, "diameter= %.3f\n\n", d_len2);
                            if (comp_count > 1) {
                                //output_print(fp, 1);
                                output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                                printf("final x pos= %f\n", x_comp_pos);
                                fprintf(fp, "final x pos= %f\n", x_comp_pos);
                                printf("final y pos= %f\n", y_comp_pos);
                                fprintf(fp, "final y pos= %f\n", y_comp_pos);
                                ///////////////////////////////////////
                                printf("diameter= %.3f\n\n", d_len2);
                                fprintf(fp, "diameter= %.3f\n\n", d_len2);
                            } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                                //printf("here\n");
                                if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                                    x_comp_pos = atof(prev_x_pos);
                                } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                                    y_comp_pos = atof(prev_y_pos);
                                }
                                //output_print(fp, 1);
                                output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                                printf("final x pos= %f\n", x_comp_pos);
                                fprintf(fp, "final x pos= %f\n", x_comp_pos);
                                printf("final y pos= %f\n", y_comp_pos);
                                fprintf(fp, "final y pos= %f\n", y_comp_pos);
                                ///////////////////////////////////////
                                printf("diameter= %.3f\n\n", d_len2);
                                fprintf(fp, "diameter= %.3f\n\n", d_len2);

                                //output_print(fp, 0);
                                output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                                printf("final x pos= %s\n", x_pos);
                                fprintf(fp, "final x pos= %s\n", x_pos);
                                printf("final y pos= %s\n\n", y_pos);
                                fprintf(fp, "final y pos= %s\n\n", y_pos);
                            }
                        } else {
                            printf("\n");
                            fprintf(fp, "\n");
                        }
                    }

                }
            //stop interpreter if end of file token "%" seen
            } else if (strcmp(token, "%") == 0) {
                //break;
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
                    //break;
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
                d_len2, comp_count, x_pos, y_pos, z_pos, x_pos2,
                feed_rate, cutter_comp_direction, i_pos, j_pos, k_pos,
                r_pos, recent_gcode, prev_x_pos, prev_y_pos,
                x_comp_pos, y_comp_pos, prev_x_pos2, prev_y_pos2,
                previous_gcode);
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (check == 0) {
                    error_printer(fp, 1, count);
                    //break;
                }

                //Check for if valid g41/42 code recently passed in
                if (strcmp(recent_gcode+1, "42") == 0 || strcmp(recent_gcode+1,"41") == 0) {
                    g41_42_check = 1;
                } else {
                    g41_42_check = 0;
                }

                //Printed out the parsed data
                //output_print(fp);
                if (strcmp(recent_gcode, "G00") == 0 || strcmp(recent_gcode, "G0") == 0 || strcmp(recent_gcode, "g0") == 0 || strcmp(recent_gcode, "g00") == 0) {
                    //output_print(fp, 0);
                    output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                    printf("final x pos= %s\n", x_pos);
                    fprintf(fp, "final x pos= %s\n", x_pos);
                    printf("final y pos= %s\n\n", y_pos);
                    fprintf(fp, "final y pos= %s\n\n", y_pos);
                } else if (strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                    strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                    strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                    //output_print(fp, 0);
                    output_print(fp, 0, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                    printf("final x pos= %s\n", x_pos);
                    fprintf(fp, "final x pos= %s\n", x_pos);
                    printf("final y pos= %s\n", y_pos);
                    fprintf(fp, "final y pos= %s\n", y_pos);
                    printf("feed rate= %s\n\n", feed_rate);
                    fprintf(fp, "feed rate= %s\n\n", feed_rate);
                } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0 ||
                    strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                    if (comp_count > 0) {
                        //output_print(fp, 1);
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                        printf("final x pos= %f\n", x_comp_pos);
                        fprintf(fp, "final x pos= %f\n", x_comp_pos);
                        printf("final y pos= %f\n", y_comp_pos);
                        fprintf(fp, "final y pos= %f\n", y_comp_pos);
                        ///////////////////////////////////////
                        printf("diameter= %.3f\n\n", d_len2);
                        fprintf(fp, "diameter= %.3f\n\n", d_len2);
                    } else if (strcmp(recent_gcode, "G40") == 0 || strcmp(recent_gcode, "g40") == 0) {
                        if (strcmp(prev_y_pos2, prev_y_pos) == 0) {
                            x_comp_pos = atof(prev_x_pos);
                        } else if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                            y_comp_pos = atof(prev_y_pos);
                        }
                        //output_print(fp, 1);
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                        printf("final x pos= %f\n", x_comp_pos);
                        fprintf(fp, "final x pos= %f\n", x_comp_pos);
                        printf("final y pos= %f\n", y_comp_pos);
                        fprintf(fp, "final y pos= %f\n", y_comp_pos);
                        ///////////////////////////////////////
                        printf("diameter= %.3f\n\n", d_len2);
                        fprintf(fp, "diameter= %.3f\n\n", d_len2);

                        //output_print(fp, 1);
                        output_print(fp, 1, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
                        x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2);
                        printf("final x pos= %s\n", x_pos);
                        fprintf(fp, "final x pos= %s\n", x_pos);
                        printf("final y pos= %s\n\n", y_pos);
                        fprintf(fp, "final y pos= %s\n\n", y_pos);
                    }
                } else {
                    printf("\n");
                    fprintf(fp, "\n");
                }
                //}
            } else {
                error_printer(fp, 1, count);
                //break;  
            }
        }
    }
    fclose(ptr);
    fclose(fp);
    return 0;
}

/*void cutter_compensation_validate(char *cutter_comp_direction, int comp_count) {
    printf("cutter comp direction = %s\n", cutter_comp_direction);
    printf("comp count = %d\n", comp_count);
    if (comp_count > 0){// && strcmp(recent_gcode, "G40") != 0) {
        //for first compensation move on x axis
        if (strcmp(prev_x_pos, x_pos) == 0 && strcmp(prev_y_pos, y_pos) != 0 && isnan(x_comp_pos)) {
            if (strcmp(cutter_comp_direction, "left") == 0) {
                x_comp_pos = atof(prev_x_pos) - (d_len2/2);
            } else if (strcmp(cutter_comp_direction, "right") == 0) {
                x_comp_pos = atof(prev_x_pos) + (d_len2/2);
            }
            //x_comp_pos = atof(prev_x_pos) - (d_len2/2);
            y_comp_pos = atof(prev_y_pos);
        //for first compensation move on y axis
        } else if (strcmp(prev_x_pos, x_pos) != 0 && strcmp(prev_y_pos, y_pos) == 0 && isnan(y_comp_pos)) {
            y_comp_pos = atof(prev_y_pos) + (d_len2/2);
            x_comp_pos = atof(prev_x_pos);
        //for point at bottom of slant/arc move to top right (clockwise and counterclockwise)
        } else if (atof(x_pos) > atof(prev_x_pos) && atof(y_pos) > atof(prev_y_pos)) {
            if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                x_comp_pos = atof(prev_x_pos) - d_len2/2;
                y_comp_pos = atof(prev_y_pos);
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) - d_len2/2;
            } else {
                float helperA = atof(x_pos) - atof(prev_x_pos);
                float helperB = atof(y_pos) - atof(prev_y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = asin(helperB/helperC) * (180/M_PI);

                if (strcmp(cutter_comp_direction, "right") == 0) {
                    helperAngle = asin(helperA/helperC) * (180/M_PI);
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (d_len2/2) * (d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    if (strcmp(prev_x_pos2, prev_x_pos) == 0) {
                        x_comp_pos = atof(prev_x_pos) - d_len2/2;
                        y_comp_pos = atof(prev_y_pos) + comp;
                    } else {
                        x_comp_pos = atof(prev_x_pos) - comp;
                        y_comp_pos = atof(prev_y_pos) - d_len2/2;
                    }

                    //x_comp_pos = atof(prev_x_pos) - d_len2/2;
                    //y_comp_pos = atof(prev_y_pos) + comp;
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    x_comp_pos = atof(prev_x_pos) + comp;
                    y_comp_pos = atof(prev_y_pos) - d_len2/2;
                }
            }
        //for point at top of slant/arc move to top right (clockwise)
        //for point at bottom of slant/arc move to bottom right (counterclockwise)
        } else if (atof(x_pos) > atof(prev_x_pos) && atof(y_pos) == atof(prev_y_pos)) {
            if (strcmp(previous_gcode, "G2") == 0 || strcmp(previous_gcode, "G02") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) + d_len2/2;
            } else if (strcmp(previous_gcode, "G3") == 0 || strcmp(previous_gcode, "G03") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) - d_len2/2;
            } else {
                float helperA = atof(prev_x_pos) - atof(prev_x_pos2);
                float helperB = atof(prev_y_pos) - atof(prev_y_pos2);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = asin(helperA/helperC) * (180/ M_PI);

                if (atof(prev_x_pos2) > atof(prev_x_pos) && atof(prev_y_pos2) > atof(prev_y_pos)) {
                    helperAngle = asin(helperB/helperC) * (180/M_PI);
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (d_len2/2) * (d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    x_comp_pos = atof(prev_x_pos) - comp;
                    y_comp_pos = atof(prev_y_pos) + d_len2/2;
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    x_comp_pos = atof(prev_x_pos) - comp;
                    y_comp_pos = atof(prev_y_pos) - d_len2/2;
                }
            }
        //for point at top of slant/arc move to bottom right (clockwise and counterclockwise) 
        } else if (atof(x_pos) > atof(prev_x_pos) && atof(y_pos) < atof(prev_y_pos)) {
            if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) + d_len2/2;
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                x_comp_pos = atof(prev_x_pos) - d_len2/2;
                y_comp_pos = atof(prev_y_pos);
            } else {
                float helperA = atof(x_pos) - atof(prev_x_pos);
                float helperB = atof(prev_y_pos) - atof(y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = asin(helperA/helperC) * (180/ M_PI);

                if (strcmp(cutter_comp_direction, "right") == 0) {
                    helperAngle = asin(helperB/helperC) * (180/M_PI);
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (d_len2/2) * (d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    x_comp_pos = atof(prev_x_pos) + comp;
                    y_comp_pos = atof(prev_y_pos) + d_len2/2;
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    x_comp_pos = atof(prev_x_pos) - d_len2/2;
                    y_comp_pos = atof(prev_y_pos) - comp;
                }
            }
        //for point at bottom of slant/arc move to bottom right (clockwise)
        //for point at bottom of slant/arc move to bottom left (counterclockwise)
        } else if (atof(x_pos) == atof(prev_x_pos) && atof(y_pos) < atof(prev_y_pos)) {
            //printf("here\n");
            if (strcmp(previous_gcode, "G2") == 0 || strcmp(previous_gcode, "G02") == 0) {
                x_comp_pos = atof(prev_x_pos) + d_len2/2;
                y_comp_pos = atof(prev_y_pos);
            } else if (strcmp(previous_gcode, "G3") == 0 || strcmp(previous_gcode, "G03") == 0) {
                //printf("inside\n");
                //if executing arc into arc move
                if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                    x_comp_pos = atof(prev_x_pos);
                    y_comp_pos = atof(prev_y_pos) - d_len2/2;
                } else  {
                    x_comp_pos = atof(prev_x_pos) - d_len2/2;
                    y_comp_pos = atof(prev_y_pos);
                }
                //x_comp_pos = atof(prev_x_pos) - d_len2/2;
                //y_comp_pos = atof(prev_y_pos);
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) + d_len2/2;
            } else {
                float helperA = atof(prev_x_pos) - atof(prev_x_pos2);
                float helperB = atof(prev_y_pos2) - atof(prev_y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngleB = asin(helperB/helperC) * (180/M_PI);
                float extraAngle = 90 - fabs(helperAngleB);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (d_len2/2) * (d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    x_comp_pos = atof(prev_x_pos) + d_len2/2;
                    y_comp_pos = atof(prev_y_pos) + comp;
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    x_comp_pos = atof(prev_x_pos) - d_len2/2;
                    y_comp_pos = atof(prev_y_pos) + comp;
                }
            }
        //for point at top of slant/arc move to bottom left (clockwise and counterclockwise)
        } else if (atof(x_pos) < atof(prev_x_pos) && atof(y_pos) < atof(prev_y_pos)) {
            if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                x_comp_pos = atof(prev_x_pos) + d_len2/2;
                y_comp_pos = atof(prev_y_pos);
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) + d_len2/2;
            } else {
                float helperA = atof(prev_x_pos) - atof(x_pos);
                float helperB = atof(prev_y_pos) - atof(y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = asin(helperB/helperC) * (180/M_PI);

                if (strcmp(cutter_comp_direction, "right") == 0) {
                    helperAngle = asin(helperA/helperC) * (180/M_PI);
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (d_len2/2) * (d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    x_comp_pos = atof(prev_x_pos) + d_len2/2;
                    y_comp_pos = atof(prev_y_pos) - comp;
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    x_comp_pos = atof(prev_x_pos) - comp;
                    y_comp_pos = atof(prev_y_pos) + d_len2/2;
                }
            }
        //for point at bottom of slant/arc move to bottom left (clockwise)
        //for point at top of slant/arc move to top left (counterclockwise)
        } else if (atof(x_pos) < atof(prev_x_pos) && atof(y_pos) == atof(prev_y_pos)) {
            //  printf("here\n");
            if (strcmp(previous_gcode, "G2") == 0 || strcmp(previous_gcode, "G02") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) - d_len2/2;
                if (strcmp(cutter_comp_direction, "right") == 0) {
                    x_comp_pos = atof(prev_x_pos);
                    y_comp_pos = atof(prev_y_pos) + d_len2/2;
                }
            } else if (strcmp(previous_gcode, "G3") == 0 || strcmp(previous_gcode, "G03") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) + d_len2/2;
            } else {
                float helperA = atof(prev_x_pos2) - atof(prev_x_pos);
                float helperB = atof(prev_y_pos2) - atof(prev_y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngleA = asin(helperA/helperC) * (180/M_PI);
                float extraAngle = 90 - fabs(helperAngleA);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (d_len2/2) * (d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    x_comp_pos = atof(prev_x_pos) + comp;
                    y_comp_pos = atof(prev_y_pos) - d_len2/2;
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    x_comp_pos = atof(prev_x_pos) + comp;
                    y_comp_pos = atof(prev_y_pos) + d_len2/2;
                }
            }
        //for point at bottom of slant/arc move to top left (clockwise and counterclockwise)
        } else if (atof(x_pos) < atof(prev_x_pos) && atof(y_pos) > atof(prev_y_pos)) {
            if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) - d_len2/2;
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                x_comp_pos = atof(prev_x_pos) + d_len2/2;
                y_comp_pos = atof(prev_y_pos);
            } else {
                float helperA = atof(prev_x_pos) - atof(x_pos);
                float helperB = atof(y_pos) - atof(prev_y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = asin(helperA/helperC) * (180/M_PI);

                if (strcmp(cutter_comp_direction, "right") == 0) {
                    helperAngle = asin(helperB/helperC) * (180/M_PI);
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (d_len2/2) * (d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    x_comp_pos = atof(prev_x_pos) - comp;
                    y_comp_pos = atof(prev_y_pos) - d_len2/2;
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    x_comp_pos = atof(prev_x_pos) + d_len2/2;
                    y_comp_pos = atof(prev_y_pos) + comp;
                }
            }
        //for point at top of slant/arc move to top left (clockwise)
        //for point at top of slant/arc move to top right (counterclockwise)
        } else if (atof(x_pos) == atof(prev_x_pos) && atof(y_pos) > atof(prev_y_pos)) {
            if (strcmp(previous_gcode, "G2") == 0 || strcmp(previous_gcode, "G02") == 0) {
                //printf("inside\n");
                x_comp_pos = atof(prev_x_pos) - d_len2/2;
                y_comp_pos = atof(prev_y_pos);
            } else if (strcmp(previous_gcode, "G3") == 0 || strcmp(previous_gcode, "G03") == 0) {
                //if executing arc into arc move
                if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                    x_comp_pos = atof(prev_x_pos);
                    y_comp_pos = atof(prev_y_pos) - d_len2/2;
                } else  {
                    x_comp_pos = atof(prev_x_pos) + d_len2/2;
                    y_comp_pos = atof(prev_y_pos);
                }
                //x_comp_pos = atof(prev_x_pos) + d_len2/2;
                //y_comp_pos = atof(prev_y_pos);
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                x_comp_pos = atof(prev_x_pos);
                y_comp_pos = atof(prev_y_pos) + d_len2/2;
            } else {
                float helperA = atof(prev_x_pos2) - atof(prev_x_pos);
                float helperB = atof(prev_y_pos) - atof(prev_y_pos2);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngleB = asin(helperB/helperC) * (180/M_PI);
                float extraAngle = 90 - fabs(helperAngleB);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (d_len2/2) * (d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    x_comp_pos = atof(prev_x_pos) - d_len2/2;
                    y_comp_pos = atof(prev_y_pos) - comp;
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    x_comp_pos = atof(prev_x_pos) + d_len2/2;
                    y_comp_pos = atof(prev_y_pos) - comp;
                }
            }
        }
    //If next G code is G40 
    } else if (comp_count == -1) {
        float x_comp = (d_len2/2) * sin(45 * (M_PI/180));
        float y_comp = sqrt(((d_len2/2) * (d_len2/2)) - x_comp * x_comp); 
        //for point at bottom of slant/arc move to bottom left or coming from horizontal line (clockwise)
        if (atof(prev_x_pos) < atof(prev_x_pos2) && atof(prev_y_pos) <= atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "left") == 0) {
            x_comp_pos = atof(prev_x_pos) + x_comp;
            y_comp_pos = atof(prev_y_pos) - y_comp;
        //for point at top of slant/arc move to top left or coming from vertical line (clockwise)
        } else if (atof(prev_x_pos) <= atof(prev_x_pos2) && atof(prev_y_pos) > atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "left") == 0) {
            x_comp_pos = atof(prev_x_pos) - x_comp;
            y_comp_pos = atof(prev_y_pos) - y_comp;
        //for point at top of slant/arc move to top right or coming from horizontal line (clockwise)
        } else if (atof(prev_x_pos) > atof(prev_x_pos2) && atof(prev_y_pos) >= atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "left") == 0) {
            x_comp_pos = atof(prev_x_pos) - x_comp;
            y_comp_pos = atof(prev_y_pos) + y_comp;
        //for point at bottom of slant/arc move to bottom right or coming from vertical line (clockwise)
        } else if (atof(prev_x_pos) >= atof(prev_x_pos2) && atof(prev_y_pos) < atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "left") == 0) {
            x_comp_pos = atof(prev_x_pos) + x_comp;
            y_comp_pos = atof(prev_y_pos) + y_comp;
        //for point at top of slant/arc move to top left or coming from horizontal line (counterclockwise)
        } else if (atof(prev_x_pos) < atof(prev_x_pos2) && atof(prev_y_pos) >= atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "right") == 0) {
            x_comp_pos = atof(prev_x_pos) + x_comp;
            y_comp_pos = atof(prev_y_pos) + y_comp;
        //for point at bottom of slant/arc move to bottom left or coming from vertical line (counterclockwise)
        } else if (atof(prev_x_pos) <= atof(prev_x_pos2) && atof(prev_y_pos) < atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "right") == 0) {
            x_comp_pos = atof(prev_x_pos) - x_comp;
            y_comp_pos = atof(prev_y_pos) + y_comp;
        //for point at bottom of slant/arc move to bottom right or coming from horizontal line (counterclockwise)
        } else if (atof(prev_x_pos) > atof(prev_x_pos2) && atof(prev_y_pos) <= atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "right") == 0) {
            x_comp_pos = atof(prev_x_pos) - x_comp;
            y_comp_pos = atof(prev_y_pos) - y_comp;
        //for point at top of slant/arc move to top right or coming from vertical line (counterclockwise)
        } else if (atof(prev_x_pos) >= atof(prev_x_pos2) && atof(prev_y_pos) > atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "right") == 0) {
            x_comp_pos = atof(prev_x_pos) + x_comp;
            y_comp_pos = atof(prev_y_pos) - y_comp;
        }
    }
}*/

// Assuming you have an enum to represent the planes
/*typedef enum {
    XY_PLANE = 0,
    XZ_PLANE,
    YZ_PLANE
} Plane;

// Variable to keep track of the current plane
Plane current_plane = XY_PLANE;

// Function to set the current plane based on the G-code
void set_plane(const char* gcode) {
    if (strcmp(gcode, "G17") == 0) {
        current_plane = XY_PLANE;
    } else if (strcmp(gcode, "G18") == 0) {
        current_plane = XZ_PLANE;
    } else if (strcmp(gcode, "G19") == 0) {
        current_plane = YZ_PLANE;
    }
}

//gcode = Gcode passed in
//coords = array of X,Y,Z passed in
//len = number of arguments passed to coords
//diameter seen = if a valid g41/42 code has been passed before (1 = yes, 0 = no)
//function returns an int "check" 
//if check == 0
//then stop and print error, else continue
int gcode_parse(char *gcode, char *coords[], int len, int diameter_seen, FILE *fp) {
    //strcpy(feed_rate, "n/a");
    static char *ret[7];
    int check = 0;
    int valid_input = 1;
    //if statement for handling "G00" codes only
    if (strcmp(gcode, "G00") == 0 || strcmp(gcode, "G0") == 0 || strcmp(gcode, "g0") == 0 || strcmp(gcode, "g00") == 0 ) {
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
                d_len2 = 0;
                comp_count = 0;
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
                //break;
            }
            if (valid_input){
                //change X's coordinates if necessary
                if (track == 0) {
                    strcpy(x_pos, ret[i]);
                    x_pos2 = atof(ret[i]);
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
                //break;
            }

            //change X's coordinates if necessary
            if (track == 0) {
                strcpy(x_pos, ret[i]);
                x_pos2 = atof(ret[i]);
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
                    //break;
                }
            }

        }
        if(!feedrate_given) {
            if(strcmp(feed_rate, "n/a") == 0){
                printf("Error: feed rate not given for g1 command \n");
                fprintf(fp, "Error: feed rate not given for g1 command \n");
                check = 0;
                //break;
            }
        }
        if (d_len2 > 0) {
            cutter_compensation_validate(cutter_comp_direction, comp_count);
            comp_count = comp_count + 1;
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
                //break;
            }

            //change X's coordinates if necessary
            if (track == 0) {
                strcpy(x_pos, ret[i]);
                x_pos2 = atof(ret[i]);
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
                    //break;
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
        }

        // chooses current_plane based on G17,18,19 
        switch (current_plane) {
            case XY_PLANE:
                // Validate for XY plane
                if (!x_given || !y_given) {
                    if(strcmp(x_pos, "n/a") == 0){
                    printf("Error: X position not given for g2 or g3 command \n");
                    fprintf(fp, "Error: X position not given for g2 or g3 command \n");
                    check = 0;
                }
                if(strcmp(y_pos, "n/a") == 0){
                    printf("Error: Y position not given for g2 or g3 command \n");
                    fprintf(fp, "Error: Y position not given for g2 or g3 command \n");
                    check = 0;
                }
                else{
                    if(strcmp(i_pos, "n/a")== 0 && strcmp(j_pos, "n/a")== 0 && i_given == 0 && j_given == 0){
                    printf("Error:  i or j not given for g2 or g3 command \n");
                    fprintf(fp, "Error:  i or j not given for g2 or g3 command \n");
                    check = 0;
                    }
                }
                }
                else{
                if(i_given == 0 && j_given == 0){
                    if(strcmp(i_pos, "n/a")== 0 && strcmp(j_pos, "n/a")== 0){
                    printf("Error:  i or j not given for g2 or g3 command \n");
                    fprintf(fp, "Error:  i or j not given for g2 or g3 command \n");
                    check = 0;
                    }
                }
            }

            if(!z_given){
                strcpy(z_pos,"0");
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

            if(!y_given){
                strcpy(y_pos,"0");
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

            if(!x_given){
                strcpy(x_pos,"0");
                x_pos2 = 0;
            } else {
                break;
            }
        }

    
        if (strcmp(gcode, "G17") == 0) {  // XY Plane
            if (i_given == 0 && j_given == 0) {
                printf("Error: I or J value required for G2/G3 command in XY plane\n");
                fprintf(fp, "Error: I or J value required for G2/G3 command in XY plane\n");
            }
            if (k_given != 0) {
                printf("Warning: K value is not used in XY plane\n");
                fprintf(fp, "Warning: K value is not used in XY plane\n");
            }
        } else if (strcmp(gcode, "G18") == 0) {  // XZ Plane
            if (i_given == 0 && k_given == 0) {
                printf("Error: I or K value required for G2/G3 command in XZ plane\n");
                fprintf(fp, "Error: I or K value required for G2/G3 command in XZ plane\n");
            }
            if (j_given != 0) {
                printf("Warning: J value is not used in XZ plane\n");
                fprintf(fp, "Warning: J value is not used in XZ plane\n");
            }
        } else if (strcmp(gcode, "G19") == 0) {  // YZ Plane
            if (j_given == 0 && k_given == 0) {
                printf("Error: J or K value required for G2/G3 command in YZ plane\n");
                fprintf(fp, "Error: J or K value required for G2/G3 command in YZ plane\n");
            }
            if (i_given != 0) {
                printf("Warning: I value is not used in YZ plane\n");
                fprintf(fp, "Warning: I value is not used in YZ plane\n");
            }
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

        if (d_len2 > 0) {
            cutter_compensation_validate(cutter_comp_direction, comp_count);
            comp_count = comp_count + 1;
        }
    //If statement for handling g40 codes only
    } else if ((strcmp(gcode, "G40") == 0 || strcmp(gcode, "g40") == 0) ||
        ((strcmp(gcode+1,"01") == 0 || strcmp(gcode+1,"1") == 0) && 
        (strcmp(coords[0], "g40") == 0 || strcmp(coords[0], "G40") == 0))) {
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
            } else if ((strcmp(coords[i], "g1") == 0 || strcmp(coords[i], "G1") || 
                strcmp(coords[i], "g01") == 0 || strcmp(coords[i], "G01") ||
                strcmp(coords[i], "g40") == 0 || strcmp(coords[i], "G40")) && i==0) {
                comp_count = -1;
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
                //break;
            }
            if (valid_input){
                //change X's coordinates if necessary
                if (track == 0) {
                    strcpy(x_pos, ret[i]);
                    x_pos2 = atof(ret[i]);
                //change Y's coordinates if necessary
                } else if (track == 1) {
                    strcpy(y_pos, ret[i]);
                } 
            }
        }
        cutter_compensation_validate(cutter_comp_direction, comp_count);
        strcpy(cutter_comp_direction, "n/a");
        d_len2 = 0;
        comp_count = 0;
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
            //char *temp = coords[i];
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
                //break;
            }

            if (check != 0 && track == 3) {
                float num;
                sscanf(ret[i], "%f", &num);
                if (signbit(num) != 0 || num > 200) {
                    check = 0;
                    //break;
                }
            }

            //change X's coordinates if necessary
            if (track == 0) {
                strcpy(x_pos, ret[i]);
                x_pos2 = atof(ret[i]);
            //change Y's coordinates if necessary
            } else if (track == 1) {
                strcpy(y_pos, ret[i]);
            //change feedrate if necessary
            } else if (track == 2) {
                strcpy(feed_rate, ret[i]);
            //change diameter value if necessary
            } else if (track == 3){
                d_len2 = atof(ret[i]);
            }
        }
        if ((diameter_given != 1 || x_or_y_given != 1) && diameter_seen == 0) {
            check = 0;
            //break;
        }
        cutter_compensation_validate(cutter_comp_direction, comp_count);
        comp_count = comp_count + 1;
    }
    return check;
}*/
