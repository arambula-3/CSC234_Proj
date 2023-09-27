#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFERSIZE (256)

int gcode_parse(char *gcode, char *coords[], int len);
int mcode_parse();
char *x_pos = "";
char *y_pos = "";
char *z_pos = "";
char *final_coords[3];
char curr_tool[BUFFERSIZE];
char *spindle_speed = "";

int main(void) //int argc, char *argv[]
{
    FILE* ptr;
    char buffer[BUFFERSIZE];
    int count = 0;

    //Will need to change file path to your own local text file
    ptr = fopen("C:/Users/aztec/OneDrive/Documents/CSC 234/test.txt", "r");

    if (NULL == ptr) {
        printf("No file detected \n");
    }
    
    printf("File contents: \n");

    while (fgets(buffer, BUFFERSIZE, ptr) != NULL) {
        //check to see if program starts with a "%"
        if (count == 0) {
            //if it doesn't throw an error
            if (strcmp(buffer, "%\n") && strcmp(buffer, "% \n") != 0) {
                printf("not proper file structure \n");     
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
                //printf("g = %s\n", gcode);
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
                //printf("check = %d\n", check);
                //Check to see if commands followed proper syntax and form
                //If not stop and print error
                if (check == 0) {
                    printf("improper proper code inputted at line = %d\n\n", count);
                    continue;
                }
                //Printed out the parsed data
                printf("x=%s\n", final_coords[0]);
                printf("y=%s\n", final_coords[1]);
                printf("z=%s\n", final_coords[2]);
                printf("current tool=%s\n", curr_tool);
                printf("spindle speed=%s\n\n", spindle_speed);
            } else {
                printf("improper proper code inputted at line = %d\n\n", count); 
            }
        }
    }
    fclose(ptr);
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
            //Check to see that X coordinates are passed in first
            if ((*coords[i] == 88) && i == 0) {
                check = 1;
            //Check to see that Y coordinates are passed in second
            } else if ((*coords[i] == 89) && i == 1) {
                check = 1;
            //Check to see that Z coordinates are passed in last
            } else if ((*coords[i] == 90) && i == 2) {
                check = 1;
            } else {
                check = 0;
                break;
            }
            char *temp = coords[i];
            ret[i] = temp+1;
            final_coords[i] = ret[i];
        }
    }
    return check;
}