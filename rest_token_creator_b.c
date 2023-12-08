#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rest_token_creator.c"  // Make sure this path is correct

void testRestTokenCreator_SimpleCase();
void testRestTokenCreator_WithComments();
void testRestTokenCreator_SpecialCharacters();

int main() {
    printf("Starting black box testing for rest_token_creator...\n");

    testRestTokenCreator_SimpleCase();
    testRestTokenCreator_WithComments();
    testRestTokenCreator_SpecialCharacters();

    printf("Black box testing for rest_token_creator completed.\n");
    return 0;
}

void testRestTokenCreator_SimpleCase() {
    char input[] = "X10 Y20 Z30";
    char *rest_token[10];
    int index = 0;
    int in_comment = 0;
    int arr[2] = {0, 0};
    int mode = 2; // Assuming mode 2 handles coordinates

    char *token = strtok(input, " ");
    rest_token_creator(token, rest_token, index, in_comment, arr, mode);

    // Check results
    if (arr[0] == 3 && strcmp(rest_token[0], "X10") == 0 && strcmp(rest_token[1], "Y20") == 0 && strcmp(rest_token[2], "Z30") == 0) {
        printf("testRestTokenCreator_SimpleCase: Passed\n");
    } else {
        printf("testRestTokenCreator_SimpleCase: Failed\n");
    }
}

void testRestTokenCreator_WithComments() {
    char input[] = "X10 (Comment) Y20";
    char *rest_token[10];
    int index = 0;
    int in_comment = 0;
    int arr[2] = {0, 0};
    int mode = 2;

    char *token = strtok(input, " ");
    rest_token_creator(token, rest_token, index, in_comment, arr, mode);

    // Check results
    // Assuming comments are ignored, and only X10 and Y20 are processed
    if (arr[0] == 2 && strcmp(rest_token[0], "X10") == 0 && strcmp(rest_token[1], "Y20") == 0) {
        printf("testRestTokenCreator_WithComments: Passed\n");
    } else {
        printf("testRestTokenCreator_WithComments: Failed\n");
    }
}

void testRestTokenCreator_SpecialCharacters() {
    char input[] = "X10 Y@# Z$%";
    char *rest_token[10];
    int index = 0;
    int in_comment = 0;
    int arr[2] = {0, 0};
    int mode = 2;

    char *token = strtok(input, " ");
    rest_token_creator(token, rest_token, index, in_comment, arr, mode);

    // Check results
    // Assuming that special characters are included in the tokens
    if (arr[0] == 3 && strcmp(rest_token[0], "X10") == 0 && strcmp(rest_token[1], "Y@#") == 0 && strcmp(rest_token[2], "Z$%") == 0) {
        printf("testRestTokenCreator_SpecialCharacters: Passed\n");
    } else {
        printf("testRestTokenCreator_SpecialCharacters: Failed\n");
    }
}

// Add more test functions for different scenarios as necessary
