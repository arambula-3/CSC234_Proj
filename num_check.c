#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#define BUFFERSIZE (256)
#pragma once

//checks a string to see if it is a float
//returns 1 if string is a float
//return 0 if string is not a float
int num_check(char *num_str) {
    float num;
    int len;
    int check = sscanf(num_str, "%f %n", &num, &len);
    if (check == 1 && !num_str[len]) {
        return 1;
    } else {
        return 0;
    }
}