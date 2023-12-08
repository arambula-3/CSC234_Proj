#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output_print.c"

// Function to process test data from a file
void processTestDataFromFile(const char* filename);

int main() {
    printf("Starting file-based white box testing for output_print...\n");

    const char* testDataFile = "path/to/test_data.txt"; // Specify your test data file path
    processTestDataFromFile(testDataFile);

    printf("File-based white box testing for output_print completed.\n");
    return 0;
}

void processTestDataFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening test data file");
        return;
    }

    char line[BUFFERSIZE];
    while (fgets(line, BUFFERSIZE, file) != NULL) {
        int comp_pos;
        char recent_gcode[10], x_pos[10], y_pos[10], z_pos[10], i_pos[10], j_pos[10], curr_tool[10], spindle_speed[10], prev_z_pos[10], previous_gcode[10],
                        prev_i_pos[10], prev_j_pos[10], cutter_comp_direction[10];
        float x_final_pos, y_final_pos, x_comp_pos, y_comp_pos, d_len2;

        sscanf(line, "%d,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%f,%f,%f,%f,%f",
               &comp_pos, recent_gcode, x_pos, y_pos, z_pos, i_pos, j_pos, curr_tool, spindle_speed,
               &x_final_pos, &y_final_pos, &x_comp_pos, &y_comp_pos, &d_len2, prev_z_pos, previous_gcode,
                        prev_i_pos, prev_j_pos, cutter_comp_direction);

        FILE *fp = tmpfile(); // Using a temporary file for output
        output_print(fp, comp_pos, recent_gcode, x_pos, y_pos, z_pos, i_pos,
                     j_pos, curr_tool, spindle_speed, &x_final_pos, &y_final_pos,
                     x_comp_pos, y_comp_pos, d_len2, prev_z_pos, previous_gcode,
                        prev_i_pos, prev_j_pos, cutter_comp_direction);

        fclose(fp); // Close the temporary file
    }

    fclose(file); // Close the test data file
}
