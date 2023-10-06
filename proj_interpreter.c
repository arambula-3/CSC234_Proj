#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFERSIZE (256)

int gcode_parse(char *gcode, char *coords[], int len);
int mcode_parse();
char x_pos[BUFFERSIZE] = "n/a";
char y_pos[BUFFERSIZE] = "n/a";
char z_pos[BUFFERSIZE] = "n/a";
char curr_tool[BUFFERSIZE] = "n/a";
char spindle_speed[BUFFERSIZE] = "n/a";

int main(void) //int argc, char *argv[]
{
    FILE* ptr;
    char buffer[BUFFERSIZE];
    int count = 0;

    //Will need to change file path to your own local temporary text test file
    ptr = fopen("../temp_test.txt", "r");

    if (NULL == ptr) {
        printf("No file detected \n");
    }

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
    
    printf("File contents: \n");
    fprintf(fp, "File contents: \n");

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
            //check to see type of command
            if ((strcmp(token, "G00") && strcmp(token, "G0") && strcmp(token, "g0") && strcmp(token, "g00")) == 0) {
                char gcode[] = "G";
                strcat(gcode,token+1);
                token = strtok(NULL, " ");
                char *rest_token[3];
                int index = 0;
                int in_comment = 0; //tracker for if we are in a comment. 0 means not in a comment. 1 means in a comment.

                //checks to see if there is a space between a coordinate and its value
                //for example = "X 36"
                //if there is a space it will concatenate those 2 token strings "X36"
                if ((strstr(token, "x") != NULL || strstr(token, "X") != NULL || strstr(token, "y") != NULL ||
                    strstr(token, "Y") != NULL || strstr(token, "z") != NULL || strstr(token, "Z") != NULL) &&
                    strlen(token) == 1) {
                    char *concat = token;
                    token = strtok(NULL, " ");
                    strcat(concat, token);
                    token = concat;
                }

                //split rest of command string and put into "rest_token"
                while(token != NULL) {
                    //if we are in a comment ignore the string tokens
                    if (strstr(token, "(") != NULL) {
                        in_comment = 1;
                        break;
                    //finds end of comment and changes in_comment to 0 to signal out of comment
                    } else if (strstr(token, ")") != NULL) {
                        in_comment = 0;
                        break;
                    //checks to see if there is a space between a coordinate and its value
                    //for example = "X 36"
                    //if there is a space it will concatenate those 2 token strings "X36"
                    } else if ((strstr(token, "x") != NULL || strstr(token, "X") != NULL || strstr(token, "y") != NULL ||
                        strstr(token, "Y") != NULL || strstr(token, "z") != NULL || strstr(token, "Z") != NULL) &&
                        strlen(token) == 1) {
                        char *concat = token;
                        token = strtok(NULL, " ");
                        strcat(concat, token);
                        token = concat;
                        rest_token[index] = token;
                        index = index + 1;
                        token = strtok(NULL, " ");
                    } else if (in_comment == 0) {
                        rest_token[index] = token;
                        index = index + 1;
                        token = strtok(NULL, " ");
                    }
                }
                int len = index;
                index = 0;
                int check = gcode_parse(gcode, rest_token, len);
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (check == 0) {
                    printf("improper proper code inputted at line = %d\n\n", count);
                    fprintf(fp, "improper proper code inputted at line = %d\n\n", count); 
                    break;
                }
                //Printed out the parsed data
                printf("x= %s\n", x_pos);
                fprintf(fp, "x= %s\n", x_pos); 
                printf("y= %s\n", y_pos);
                fprintf(fp, "y= %s\n", y_pos); 
                printf("z= %s\n", z_pos);
                fprintf(fp, "z= %s\n", z_pos);
                printf("current tool= %s\n", curr_tool);
                fprintf(fp, "current tool= %s\n", curr_tool);
                printf("spindle speed= %s\n\n", spindle_speed);
                fprintf(fp, "spindle speed= %s\n\n", spindle_speed);
            //stop interpreter if end of file token "%" seen
            } else if (strcmp(token, "%") == 0) {
                break;
            //checks to see if line is just a comment and then ignores it
            } else if (strstr(token, "(") != NULL) {
                continue;
            } else {
                printf("improper proper code inputted at line = %d\n\n", count);
                fprintf(fp, "improper proper code inputted at line = %d\n\n", count);
                break;  
            }
        }
    }
    fclose(ptr);
    fclose(fp);
    return 0;
}

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

//gcode = Gcode passed in
//coords = array of X,Y,Z passed in
//len = number of arguments passed to coords
//function returns an int "check" if check == 0
//then stop and print error, else continue
int gcode_parse(char *gcode, char *coords[], int len) {
    static char *ret[3];
    int check = 0;
    //if statement for handling "G00" codes only
    if ((strcmp(gcode, "G00") && strcmp(gcode, "G0") && strcmp(gcode, "g0") && strcmp(gcode, "g00")) == 0) {
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
            }
        }
    }
    return check;
}