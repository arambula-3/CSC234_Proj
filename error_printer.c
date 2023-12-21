#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define BUFFERSIZE (256)

//code = type of error to throw
//line = where error located
//prints errors
void error_printer(FILE *fp, int code, int line) {
    if (code == 1) {
        printf("improper code inputted at line = %d\n\n", line);
        fprintf(fp, "improper code inputted at line = %d\n\n", line);
    } else if (code == 2) {
        printf("too many M codes on line = %d\n\n", line);
        fprintf(fp, "too many M codes on line = %d\n\n", line);
    } else if (code == 3) {
        printf("improper comment at line = %d\n\n", line);
        fprintf(fp, "improper comment at line = %d\n\n", line);
    }
}