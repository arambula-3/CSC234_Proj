#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#define BUFFERSIZE (256)
#include "num_check.c"

//mcode = Mcode passed in
//args = array of arguments passed in
//len = number of arguments passed in
//function returns an int "check" 
//if check == 0
//then stop and print error, else continue
int mcode_parse(char *mcode, char *args[], int len, char *curr_tool, char *spindle_speed) {
    static char *ret[7];
    int check = 0;
    //if statement for handling mcodes that change spindle speed
    if (strcmp(mcode, "M03") == 0 || strcmp(mcode, "M3") == 0 || strcmp(mcode, "M04") == 0 || strcmp(mcode, "M4") == 0) {
        for (int i = 0; i < len; i++) {
            int track;
            //Check to see that spindle speed is passed in
            if ((*args[i] == 83) || (*args[i] == 115))  {
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
                break;
            }

            //change spindle speed if necessary
            if (track == 0) {
                strcpy(spindle_speed, ret[i]);
            }
        }
    //if statement for handling mcodes that change tool
    } else if (strcmp(mcode, "M06") == 0 || strcmp(mcode, "M6") == 0 || strcmp(mcode, "M16") == 0) {
        strcpy(spindle_speed, "0");
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
                break;
            }

            //change tool if necessary
            if (track == 0) {
                strcpy(curr_tool, ret[i]);
            }
        }
    }
    return check;
}
