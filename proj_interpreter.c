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
            if (strcmp(token, "G00") == 0) {
                char gcode[] = "G";
                strcat(gcode,token+1);
                token = strtok(NULL, " ");
                char *rest_token[3];
                int index = 0;
                //split rest of command string and put into "rest_token"
                while(token != NULL) {
                    rest_token[index] = token;
                    index = index + 1;
                    token = strtok(NULL, " ");
                }
                int len = index;
                index = 0;
                int check = gcode_parse(gcode, rest_token, len);
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (check == 0) {
                    printf("improper proper code inputted at line = %d\n\n", count);
                    fprintf(fp, "improper proper code inputted at line = %d\n\n", count); 
                    continue;
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
            } else {
                printf("improper proper code inputted at line = %d\n\n", count);
                fprintf(fp, "improper proper code inputted at line = %d\n\n", count);  
            }
        }
    }
    fclose(ptr);
    fclose(fp);
    return 0;
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
    if (strcmp(gcode, "G00") == 0) {
        for (int i = 0; i < len; i++) {
            int track;
            //Check to see that X coordinates are passed in first
            if ((*coords[i] == 88) && i == 0) {
                check = 1;
                track = 0;
            //Check to see that Y coordinates are passed in second or first if no X coordinate given
            } else if (((*coords[i] == 89) && i == 1 && (len == 2 || len == 3)) 
                || ((*coords[i] == 89) && i == 0 && (len == 2 || len == 1))) {
                check = 1;
                track = 1;
            //Check to see that Z coordinates are passed in last or first if no other coordinates given
            } else if (((*coords[i] == 90) && i == 2 && len == 3)
                || ((*coords[i] == 90) && i == 1 && len == 2)
                || ((*coords[i] == 90) && i == 0 && len == 1)) {
                check = 1;
                track = 2;
            } else {
                check = 0;
                break;
            }
            char *temp = coords[i];
            ret[i] = temp+1;
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