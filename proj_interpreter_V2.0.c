#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define BUFFERSIZE (256)

int gcode_parse(char *gcode, char *coords[], int len, int diameter_seen);
int mcode_parse(char *mcode, char *args[], int len);
void output_print(FILE *fp);
void error_printer(FILE *fp, int code, int line);
void rest_token_creator(char *token, char *rest_token[], int index, int in_comment, int arr[], int mode);

char recent_gcode[BUFFERSIZE] = "n/a";
char x_pos[BUFFERSIZE] = "n/a";
char y_pos[BUFFERSIZE] = "n/a";
char z_pos[BUFFERSIZE] = "n/a";
char i_pos[BUFFERSIZE] = "n/a";
char j_pos[BUFFERSIZE] = "n/a";
char k_pos[BUFFERSIZE] = "n/a";
char r_pos[BUFFERSIZE] = "n/a";
char d_len[BUFFERSIZE] = "n/a";
char feed_rate[BUFFERSIZE] = "n/a";
char curr_tool[BUFFERSIZE] = "n/a";
char spindle_speed[BUFFERSIZE] = "n/a";

int main(void)
{
    FILE* ptr;
    char buffer[BUFFERSIZE];
    int count = 0;

    //Will need to change file path to your own local temporary text test file
    ptr = fopen("../temp_test2.txt", "r");

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
            count = count + 1;
            buffer[strcspn(buffer, "\n")] = 0;
            char *token = strtok(buffer, " ");

            //check to see type of g command
            if ((token[0] == 103 || token[0] == 71) && (strcmp(token+1, "00") == 0 || strcmp(token+1, "0") == 0 ||
                strcmp(token+1, "01") == 0 || strcmp(token+1, "1") == 0 || strcmp(token+1, "02") == 0 ||
                strcmp(token+1, "2") == 0 || strcmp(token+1, "03") == 0 || strcmp(token+1, "3") == 0 ||
                strcmp(token+1, "41") == 0 || strcmp(token+1, "42") == 0)){               

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
                if ((strstr(token, "x") != NULL || strstr(token, "X") != NULL || strstr(token, "y") != NULL ||
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
                }

                //Check to see if gcodes from same group are input on the same line
                int gcode_group_check = 0; //0 means gcodes are from different groups so no error
                for (int i = 0; i < index; i++) {
                    if (strstr(rest_token[i], "g") != NULL || strstr(rest_token[i], "G") != NULL) {
                        if (strstr("0010203", rest_token[i]+1) != NULL && strstr("0010203", gcode+1) != NULL) {
                            gcode_group_check = 1;
                        } else if ((strcmp("41",rest_token[i]+1) == 0 || strcmp("42",rest_token[i]+1) == 0) &&
                            (strcmp("41",gcode+1) == 0 || strcmp("42",gcode+1) == 0)) {
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
                int check = gcode_parse(gcode, rest_token, len, g41_42_check);
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
                        m_check = mcode_parse(mcode, mcode_token, mcode_len);
                    } else if (strstr(mcode_token[1],"m") != NULL || strstr(mcode_token[1],"M") != NULL) {
                        strcat(mcode,mcode_token[1]+1);
                        mcode_len = mcode_len - 1;
                        m_check = mcode_parse(mcode, mcode_token, mcode_len);
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
                output_print(fp);
                if (strcmp(gcode, "G01") == 0 || strcmp(gcode, "G1") == 0 || strcmp(gcode, "g1") == 0 || strcmp(gcode, "g01") == 0 ||
                    strcmp(gcode, "G02") == 0 || strcmp(gcode, "G2") == 0 || strcmp(gcode, "g2") == 0 || strcmp(gcode, "g02") == 0 ||
                    strcmp(gcode, "G03") == 0 || strcmp(gcode, "G3") == 0 || strcmp(gcode, "g3") == 0 || strcmp(gcode, "g03") == 0){
                    printf("feed rate= %s\n\n", feed_rate);
                    fprintf(fp, "feed rate= %s\n\n", feed_rate);
                } else if (strcmp(gcode, "G41") == 0 || strcmp(gcode, "G42") == 0 || strcmp(gcode, "g41") == 0 || strcmp(gcode, "g42") == 0) {
                    printf("diameter= %s\n\n", d_len);
                    fprintf(fp, "diameter= %s\n\n", d_len);
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
                if ((strstr(token, "t") != NULL || strstr(token, "T") != NULL || strstr(token, "s") != NULL ||
                    strstr(token, "S") != NULL) && strlen(token) == 1) {
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
                        output_print(fp);
                        if (strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                            strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                            strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                            printf("feed rate= %s\n\n", feed_rate);
                            fprintf(fp, "feed rate= %s\n\n", feed_rate);
                        } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0) {
                            printf("diameter= %s\n\n", d_len);
                            fprintf(fp, "diameter= %s\n\n", d_len);
                        }
                    }
                }

                int len = index;
                index = 0;
                int check = mcode_parse(mcode, rest_token, len);
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (check == 0 && seen_already == 0) { 
                    error_printer(fp, 1, count);
                    //break;
                }

                if (strcmp(mcode+1, "03") == 0 || strcmp(mcode+1,"04") == 0 || strcmp(mcode+1, "06") == 0 ||
                    strstr("346", mcode+1) != NULL || strcmp(mcode+1, "16") == 0) {
                    //Printed out the parsed data
                    output_print(fp);
                    if (strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                        strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                        strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                        printf("feed rate= %s\n\n", feed_rate);
                        fprintf(fp, "feed rate= %s\n\n", feed_rate);
                    } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0) {
                        printf("diameter= %s\n\n", d_len);
                        fprintf(fp, "diameter= %s\n\n", d_len);
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
                        strcat(mcode, rest_token[1]+1);
                        len = len - 1;
                        check = mcode_parse(mcode, rest_token, len);
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
                        output_print(fp);
                        if (strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                            strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                            strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                            printf("feed rate= %s\n\n", feed_rate);
                            fprintf(fp, "feed rate= %s\n\n", feed_rate);
                        } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0) {
                            printf("diameter= %s\n\n", d_len);
                            fprintf(fp, "diameter= %s\n\n", d_len);
                        }
                    }

                }
            //stop interpreter if end of file token "%" seen
            } else if (strcmp(token, "%") == 0) {
                //break;
            //checks to see if line is just a comment and that it is closed properly
            } else if (strstr(token, "(") != NULL) {
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
                int check = gcode_parse(recent_gcode, rest_token, len, g41_42_check);
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
                output_print(fp);
                if (strcmp(recent_gcode, "G01") == 0 || strcmp(recent_gcode, "G1") == 0 || strcmp(recent_gcode, "g1") == 0 || strcmp(recent_gcode, "g01") == 0 ||
                    strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "g2") == 0 || strcmp(recent_gcode, "g02") == 0 ||
                    strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "g3") == 0 || strcmp(recent_gcode, "g03") == 0){
                    printf("feed rate= %s\n\n", feed_rate);
                    fprintf(fp, "feed rate= %s\n\n", feed_rate);
                } else if (strcmp(recent_gcode, "G41") == 0 || strcmp(recent_gcode, "G42") == 0 || strcmp(recent_gcode, "g41") == 0 || strcmp(recent_gcode, "g42") == 0) {
                    printf("diameter= %s\n\n", d_len);
                    fprintf(fp, "diameter= %s\n\n", d_len);
                }
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

//token = current token
//rest_token = rest of the tokens
//index = how long rest_token is
//in_comment = if we are in a comment or not
//arr[] = stores "index" in [0] and "in_comment" in [1]
//mode = which if statement to use (1 = t,s and 2 = x,y,z,f,i,j,k,r)
void rest_token_creator(char *token, char *rest_token[], int index, int in_comment, int arr[], int mode) {
    //split rest of argument string and put into "rest_token"
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
        //for example = "t 36"
        //if there is a space it will concatenate those 2 token strings "t36"
        //mode = 1
        } else if (((strstr(token, "t") != NULL || strstr(token, "T") != NULL || strstr(token, "s") != NULL ||
            strstr(token, "S") != NULL ) && strlen(token) == 1) && mode == 1 && in_comment == 0) {
            char *concat = token;
            token = strtok(NULL, " "); 
            strcat(concat, token);
            token = concat;
            rest_token[index] = token;
            index = index + 1;
            token = strtok(NULL, " ");
        //checks to see if there is a space between a coordinate and its value
        //for example = "X 36"
        //if there is a space it will concatenate those 2 token strings "X36"
        //mode = 2
        } else if (((strstr(token, "x") != NULL || strstr(token, "X") != NULL || strstr(token, "y") != NULL ||
            strstr(token, "Y") != NULL || strstr(token, "z") != NULL || strstr(token, "Z") != NULL || 
            strstr(token, "f") != NULL || strstr(token, "F") != NULL || strstr(token, "I") != NULL ||
            strstr(token, "i") != NULL || strstr(token, "J") != NULL || strstr(token, "j") != NULL ||
            strstr(token, "K") != NULL || strstr(token, "k") != NULL || 
            strstr(token, "R") != NULL || strstr(token, "r") != NULL) && strlen(token) == 1) && mode == 2 && in_comment == 0) {
            char *concat = token;
            token = strtok(NULL, " "); 
            strcat(concat, token);
            token = concat;
            rest_token[index] = token;
            index = index + 1;
            token = strtok(NULL, " ");
        //if not in comment then add values to rest_token
        } else if (in_comment == 0 && strstr(token, "(") == NULL && strstr(token, ")") == NULL) {
            rest_token[index] = token;
            index = index + 1;
            token = strtok(NULL, " ");
        } else {
            token = strtok(NULL, " ");
        }
    }
    arr[0] = index;
    arr[1] = in_comment;
};

//checks a string to see if it is a float
//returns 1 if string is a float
//return 0 if string is not a float
int num_check(char *num_str) {
    float num;
    int check = sscanf(num_str, "%f", &num);
    if (check == 1) {
        return 1;
    } else {
        return 0;
    }
}

//prints the outputs
void output_print(FILE *fp) {
    //Printed out the parsed data
    printf("x= %s\n", x_pos);
    fprintf(fp, "x= %s\n", x_pos); 
    printf("y= %s\n", y_pos);
    fprintf(fp, "y= %s\n", y_pos); 
    printf("z= %s\n", z_pos);
    fprintf(fp, "z= %s\n", z_pos);
    //printf("i= %s\n", i_pos);
    //fprintf(fp, "i= %s\n", i_pos); 
    //printf("j= %s\n", j_pos);
    //fprintf(fp, "j= %s\n", j_pos); 
    //printf("k= %s\n", k_pos);
    //fprintf(fp, "k= %s\n", k_pos);
    printf("current tool= %s\n", curr_tool);
    fprintf(fp, "current tool= %s\n", curr_tool);
    printf("spindle speed= %s\n\n", spindle_speed);
    fprintf(fp, "spindle speed= %s\n\n", spindle_speed);
}

//code = type of error to throw
//line = where error located
//prints errors
void error_printer(FILE *fp, int code, int line) {
    if (code == 1) {
        printf("improper proper code inputted at line = %d\n\n", line);
        fprintf(fp, "improper proper code inputted at line = %d\n\n", line);
    } else if (code == 2) {
        printf("too many M codes on line = %d\n\n", line);
        fprintf(fp, "too many M codes on line = %d\n\n", line);
    } else if (code == 3) {
        printf("improper comment at line = %d\n\n", line);
        fprintf(fp, "improper comment at line = %d\n\n", line);
    }
}

//gcode = Gcode passed in
//coords = array of X,Y,Z passed in
//len = number of arguments passed to coords
//diameter seen = if a valid g41/42 code has been passed before (1 = yes, 0 = no)
//function returns an int "check" 
//if check == 0
//then stop and print error, else continue
int gcode_parse(char *gcode, char *coords[], int len, int diameter_seen) {
    //strcpy(feed_rate, "n/a");
    static char *ret[7];
    int check = 0;
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
            //change Y's coordinates if necessary
            } else if (track == 1) {
                strcpy(y_pos, ret[i]);
            //change Z's coordinates if necessary
            } else if (track == 2) {
                strcpy(z_pos, ret[i]);
            //change F's coordinates if necessary
            } 
        }
    //if statement for handling "G01" codes only
    } else if ((strcmp(gcode, "G01") == 0 || strcmp(gcode, "G1") == 0 || strcmp(gcode, "g1") == 0 || strcmp(gcode, "g01") == 0) &&
        (strcmp("g41",coords[0]) != 0 && strcmp("G41",coords[0]) != 0 && 
        strcmp("g42",coords[0]) != 0 && strcmp("G42",coords[0]) != 0)) {
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
            //change Y's coordinates if necessary
            } else if (track == 1) {
                strcpy(y_pos, ret[i]);
            //change Z's coordinates if necessary
            } else if (track == 2) {
                strcpy(z_pos, ret[i]);
            //change F's coordinates if necessary
            } else if (track == 3){
                strcpy(feed_rate, ret[i]);
            }
        }
    //if statement for handling "G02" and "G03" codes only
    } else if (strcmp(gcode, "G02") == 0 || strcmp(gcode, "G2") == 0 || strcmp(gcode, "g2") == 0 || strcmp(gcode, "g02") == 0 ||
        strcmp(gcode, "G03") == 0 || strcmp(gcode, "G3") == 0 || strcmp(gcode, "g3") == 0 || strcmp(gcode, "g03") == 0) {
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
            //change Y's coordinates if necessary
            } else if (track == 1) {
                strcpy(y_pos, ret[i]);
            //change Z's coordinates if necessary
            } else if (track == 2) {
                strcpy(z_pos, ret[i]);
            //change F's coordinates if necessary
            } else if (track == 3){
                strcpy(feed_rate, ret[i]);
            } else if (track == 4) {
                strcpy(i_pos, ret[i]);
            } else if (track == 5) {
                strcpy(j_pos, ret[i]);
            } else if (track == 6) {
                strcpy(k_pos, ret[i]);
            } else if (track == 7) {
                strcpy(r_pos, ret[i]);
            }  
        }
    //If statement for handling g41/42 codes only
    } else if ((strcmp(gcode, "G41") == 0 || strcmp(gcode, "G42") == 0 || strcmp(gcode, "g41") == 0 || strcmp(gcode, "g42") == 0) ||
        (strcmp(gcode+1,"01") == 0 || strcmp(gcode+1,"1") == 0)){
        int diameter_given = 0;
        int x_or_y_given = 0;
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
            //change Y's coordinates if necessary
            } else if (track == 1) {
                strcpy(y_pos, ret[i]);
            //change feedrate if necessary
            } else if (track == 2) {
                strcpy(feed_rate, ret[i]);
            //change diameter value if necessary
            } else if (track == 3){
                strcpy(d_len, ret[i]);
            }
        }
        if ((diameter_given != 1 || x_or_y_given != 1) && diameter_seen == 0) {
            check = 0;
            //break;
        }
    }
    return check;
}

//mcode = Mcode passed in
//args = array of arguments passed in
//len = number of arguments passed in
//function returns an int "check" 
//if check == 0
//then stop and print error, else continue
int mcode_parse(char *mcode, char *args[], int len) {
    //strcpy(feed_rate, "n/a");
    static char *ret[7];
    int check = 0;
    //if statement for handling mcodes that change spindle speed
    if (strcmp(mcode, "M03") == 0 || strcmp(mcode, "M3") == 0 || strcmp(mcode, "M04") == 0 || strcmp(mcode, "M4") == 0) {
        for (int i = 0; i < len; i++) {
            int track;
            //Check to see that spindle speed is passed in
            if ((*args[i] == 83) || (*args[i] == 115)) {
                check = 1;
                track = 0;
            } else {
                check = 0;
                break;
            }
            char *temp = args[i];
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

            //change spindle speed if necessary
            if (track == 0) {
                strcpy(spindle_speed, ret[i]);
            }
        }
    //if statement for handling mcodes that change tool
    } else if (strcmp(mcode, "M06") == 0 || strcmp(mcode, "M6") == 0 || strcmp(mcode, "M16") == 0) {
        for (int i = 0; i < len; i++) {
            int track;
            //Check to see that tool is passed in
            if ((*args[i] == 84) || (*args[i] == 116)) {
                check = 1;
                track = 0;
            } else {
                check = 0;
                break;
            }
            char *temp = args[i];
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

            //change tool if necessary
            if (track == 0) {
                strcpy(curr_tool, ret[i]);
            }
        }
    }
    return check;
}