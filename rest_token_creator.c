#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#define BUFFERSIZE (256)

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
}