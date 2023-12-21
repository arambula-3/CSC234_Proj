#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../rest_token_creator.c"  // Include the rest_token_creator function

void testRestTokenCreator_HandleComments();
void testRestTokenCreator_HandleModeOne();
void testRestTokenCreator_HandleModeTwo();
void testRestTokenCreator_NoComments();

int main() {
    printf("Starting white box testing for rest_token_creator...\n");

    testRestTokenCreator_HandleComments();
    testRestTokenCreator_HandleModeOne();
    testRestTokenCreator_HandleModeTwo();
    testRestTokenCreator_NoComments();

    printf("White box testing for rest_token_creator completed.\n");
    return 0;
}

void testRestTokenCreator_HandleComments() {
    char input[] = "X10 (This is a comment) Y20";
    char *rest_token[10];
    int index = 0;
    int in_comment = 0;
    int arr[2] = {0, 0};
    int mode = 2;

    char *token = strtok(input, " ");
    rest_token_creator(token, rest_token, index, in_comment, arr, mode);

    // Assuming comments are ignored, and only X10 and Y20 are processed
    if (arr[0] == 2 && strcmp(rest_token[0], "X10") == 0 && strcmp(rest_token[1], "Y20") == 0) {
        printf("testRestTokenCreator_HandleComments: Passed\n");
    } else {
        printf("testRestTokenCreator_HandleComments: Failed\n");
    }
}

void testRestTokenCreator_HandleModeOne() {
    char input[] = "T 36 S 500";
    char *rest_token[10];
    int index = 0;
    int in_comment = 0;
    int arr[2] = {0, 0};
    int mode = 1;

    char *token = strtok(input, " ");
    rest_token_creator(token, rest_token, index, in_comment, arr, mode);

    // Assuming mode 1 concatenates tokens like "T 36" into "T36"
    if (arr[0] == 2 && strcmp(rest_token[0], "T36") == 0 && strcmp(rest_token[1], "S500") == 0) {
        printf("testRestTokenCreator_HandleModeOne: Passed\n");
    } else {
        printf("testRestTokenCreator_HandleModeOne: Failed\n");
    }
}

void testRestTokenCreator_HandleModeTwo() {
    char input[] = "X 36 Y 20";
    char *rest_token[10];
    int index = 0;
    int in_comment = 0;
    int arr[2] = {0, 0};
    int mode = 2;

    char *token = strtok(input, " ");
    rest_token_creator(token, rest_token, index, in_comment, arr, mode);

    // Assuming mode 2 concatenates tokens like "X 36" into "X36"
    if (arr[0] == 2 && strcmp(rest_token[0], "X36") == 0 && strcmp(rest_token[1], "Y20") == 0) {
        printf("testRestTokenCreator_HandleModeTwo: Passed\n");
    } else {
        printf("testRestTokenCreator_HandleModeTwo: Failed\n");
    }
}

void testRestTokenCreator_NoComments() {
    char input[] = "X36 Y20 Z10";
    char *rest_token[10];
    int index = 0;
    int in_comment = 0;
    int arr[2] = {0, 0};
    int mode = 2;

    char *token = strtok(input, " ");
    rest_token_creator(token, rest_token, index, in_comment, arr, mode);

    // Check if the output correctly handles a string with no comments
    if (arr[0] == 3 && strcmp(rest_token[0], "X36") == 0 && strcmp(rest_token[1], "Y20") == 0 && strcmp(rest_token[2], "Z10") == 0) {
        printf("testRestTokenCreator_NoComments: Passed\n");
    } else {
        printf("testRestTokenCreator_NoComments: Failed\n");
    }
}

// Define more test functions for other scenarios and paths
