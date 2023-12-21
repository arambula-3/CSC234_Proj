#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#define BUFFERSIZE (256) 

//prints the outputs
void output_print(FILE *fp, int comp_pos, char *recent_gcode, char *x_pos, char *y_pos, char *z_pos, char *i_pos,
    char *j_pos, char *curr_tool, char *spindle_speed, float *x_final_pos, float *y_final_pos, float x_comp_pos, 
    float y_comp_pos, float d_len2, char *prev_z_pos, char *previous_gcode, char *prev_i_pos, char *prev_j_pos,
    char *cutter_comp_direction) {
    //X and Y final position calculation
    float prev_x_final_pos = *x_final_pos;
    float prev_y_final_pos = *y_final_pos;
    if (strcmp(recent_gcode, "G40") == 0 || comp_pos == 1) {
        if (comp_pos == 1) {
            *x_final_pos = x_comp_pos;
            *y_final_pos = y_comp_pos;
        } else {
            *x_final_pos = atof(x_pos);
            *y_final_pos = atof(y_pos);
            if (strcmp(x_pos,"n/a") == 0) {
                *x_final_pos = NAN;
            }
            if (strcmp(y_pos,"n/a") == 0) {
                *y_final_pos = NAN;
            }
        }
    } else if (d_len2 == 0 || isnan(d_len2)) {
        *x_final_pos = atof(x_pos);
        *y_final_pos = atof(y_pos);
        if (strcmp(x_pos,"n/a") == 0) {
            *x_final_pos = NAN;
        }
        if (strcmp(y_pos,"n/a") == 0) {
            *y_final_pos = NAN;
        }
    } else {
        *x_final_pos = x_comp_pos;
        *y_final_pos = y_comp_pos;
    }
    ////intermediate points calculation/////
    if ((prev_x_final_pos != *x_final_pos && !isnan(prev_x_final_pos)) || 
    (prev_y_final_pos != *y_final_pos && !isnan(prev_y_final_pos)) || strcmp(prev_z_pos, z_pos) != 0 ||
    ((strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || 
    strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0) && 
    prev_x_final_pos == *x_final_pos && prev_y_final_pos == *y_final_pos && (d_len2 == 0 || isnan(d_len2))) ||
    ((strcmp(previous_gcode, "G02") == 0 || strcmp(previous_gcode, "G2") == 0 || 
    strcmp(previous_gcode, "G03") == 0 || strcmp(previous_gcode, "G3") == 0) && 
    prev_x_final_pos == *x_final_pos && prev_y_final_pos == *y_final_pos && d_len2 > 0)) {
        //Intermediate points for straight lines
        if ((strcmp(recent_gcode, "G02") != 0 && strcmp(recent_gcode, "G2") != 0 && 
        strcmp(recent_gcode, "G03") != 0 && strcmp(recent_gcode, "G3") != 0 && (d_len2 == 0 || isnan(d_len2))) || 
        (strcmp(previous_gcode, "G02") != 0 && strcmp(previous_gcode, "G2") != 0 && 
        strcmp(previous_gcode, "G03") != 0 && strcmp(previous_gcode, "G3") != 0 && d_len2 > 0)){
            int index = 1;
            float x_inter_pos = NAN;
            float y_inter_pos = NAN;
            float z_inter_pos = NAN;
            while (index < 1000) {
                //X position changes
                if (isnan(prev_x_final_pos)) {
                    float x_pos_diff = *x_final_pos - 0;
                    x_inter_pos = 0 + (x_pos_diff/1000) * index;
                } else {
                    float x_pos_diff = *x_final_pos - prev_x_final_pos;
                    x_inter_pos = prev_x_final_pos + (x_pos_diff/1000) * index;
                }
                //Y position changes
                if (isnan(prev_y_final_pos)) {
                    float y_pos_diff = *y_final_pos - 0;
                    y_inter_pos = 0 + (y_pos_diff/1000) * index;
                } else {
                    float y_pos_diff = *y_final_pos -prev_y_final_pos;
                    y_inter_pos = prev_y_final_pos + (y_pos_diff/1000) * index;
                }
                //Z position changes
                if (strcmp(prev_z_pos,"n/a") == 0) {
                    float z_pos_diff = atof(z_pos) - 0;
                    z_inter_pos = 0 + (z_pos_diff/1000) * index;
                } else {
                    float z_pos_diff = atof(z_pos) - atof(prev_z_pos);
                    z_inter_pos = atof(prev_z_pos) + (z_pos_diff/1000) * index;
                }
                printf("| X Position | Y Position | Z Position | Current Tool | Spindle Speed |\n");
                fprintf(fp, "| X Position | Y Position | Z Position | Current Tool | Spindle Speed |\n");
                printf("| %.3f | %.3f | %.3f | %s | %s |\n", x_inter_pos, y_inter_pos, z_inter_pos, curr_tool, spindle_speed);
                fprintf(fp, "| %.3f | %.3f | %.3f | %s | %s |\n", x_inter_pos, y_inter_pos, z_inter_pos, curr_tool, spindle_speed);
                index = index + 1;
            }
        //Intermediate points for arcs
        } else if (((strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0 || 
        strcmp(recent_gcode, "G03") == 0 || strcmp(recent_gcode, "G3") == 0) && (d_len2 == 0 || isnan(d_len2))) ||
        ((strcmp(previous_gcode, "G02") == 0 || strcmp(previous_gcode, "G2") == 0 || 
        strcmp(previous_gcode, "G03") == 0 || strcmp(previous_gcode, "G3") == 0) && d_len2 > 0)){
            int index = 1;
            float x_inter_pos = NAN;
            float y_inter_pos = NAN;
            float z_inter_pos = NAN;
            float x_arc_center = NAN;
            float y_arc_center = NAN;
            float radius = NAN;
            float central_angle = NAN;

            //calculate radius
            if (d_len2 == 0 || isnan(d_len2)) {
                if (atof(i_pos) != 0 && atof(j_pos) != 0) {
                    radius = sqrt(fabs(atof(i_pos)) * fabs(atof(i_pos)) + fabs(atof(j_pos)) * fabs(atof(j_pos)));
                } else if (atof(i_pos) == 0) {
                    radius = fabs(atof(j_pos));
                } else if (atof(j_pos) == 0) {
                    radius = fabs(atof(i_pos));
                }
            } else {
                if (atof(prev_i_pos) != 0 && atof(prev_j_pos) != 0) {
                    radius = sqrt(fabs(atof(prev_i_pos)) * fabs(atof(prev_i_pos)) + fabs(atof(prev_j_pos)) * fabs(atof(prev_j_pos)));
                } else if (atof(prev_i_pos) == 0) {
                    radius = fabs(atof(prev_j_pos));
                } else if (atof(prev_j_pos) == 0) {
                    radius = fabs(atof(prev_i_pos));
                }
            }

            //find center of arc x,y
            if (strcmp(recent_gcode, "G40") == 0 || comp_pos == 1) {
                //adjust for compensation
                if (comp_pos == 1) {
                    //clockwise arc
                    if (strcmp(previous_gcode, "G02") == 0 || strcmp(previous_gcode, "G2") == 0){
                         //cutter comp right
                        if (strcmp(cutter_comp_direction, "right") == 0) {
                            if (atof(prev_i_pos)>0) {
                                x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(-1);
                            } else if (atof(prev_i_pos)<0){
                                x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(1);
                            } else {
                                x_arc_center = prev_x_final_pos;
                            }
                            if (atof(prev_j_pos)>0) {
                                y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(-1);
                            } else if(atof(prev_i_pos)<0) {
                                y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(1);
                            } else {
                                y_arc_center = prev_y_final_pos;
                            }
                            radius = radius - d_len2/2;
                        //cutter comp left
                        } else {
                            if (atof(prev_i_pos)>0) {
                                x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(1);
                            } else if (atof(prev_i_pos)<0) {
                                x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(-1);
                            } else {
                                x_arc_center = prev_x_final_pos;
                            }
                            if (atof(prev_j_pos)>0) {
                                y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(1);
                            } else if (atof(prev_j_pos)<0) {
                                y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(-1);
                            } else {
                                y_arc_center = prev_y_final_pos;
                            }
                            radius = radius + d_len2/2;
                        }
                    //counterclockwise arc
                    } else {
                        //cutter comp right
                        if (strcmp(cutter_comp_direction, "right") == 0) {
                            if (atof(prev_i_pos)>0) {
                                x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(1);
                            } else if (atof(prev_i_pos)<0) {
                                x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(-1);
                            } else {
                                x_arc_center = prev_x_final_pos;
                            }
                            if (atof(prev_j_pos)>0) {
                                y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(1);
                            } else if (atof(prev_j_pos)<0) {
                                y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(-1);
                            } else {
                                y_arc_center = prev_y_final_pos;
                            }
                            radius = radius + d_len2/2;
                        //cutter comp left
                        } else {
                            if (atof(prev_i_pos)>0) {
                                x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(-1);
                            } else if (atof(prev_i_pos)<0){
                                x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(1);
                            } else {
                                x_arc_center = prev_x_final_pos;
                            }
                            if (atof(prev_j_pos)>0) {
                                y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(-1);
                            } else if (atof(prev_j_pos)<0) {
                                y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(1);
                            } else {
                                y_arc_center = prev_y_final_pos;
                            }
                            radius = radius - d_len2/2;
                        }
                    }
                } else if (d_len2 == 0 || isnan(d_len2)) {
                    x_arc_center = prev_x_final_pos + atof(i_pos);
                    y_arc_center = prev_y_final_pos + atof(j_pos);
                } else {
                    x_arc_center = prev_x_final_pos + atof(prev_i_pos);
                    y_arc_center = prev_y_final_pos + atof(prev_j_pos);
                }
            } else if (d_len2 == 0 || isnan(d_len2)) {
                x_arc_center = prev_x_final_pos + atof(i_pos);
                y_arc_center = prev_y_final_pos + atof(j_pos);
            //adjust for compensation
            } else {
                //clockwise arc
                if (strcmp(previous_gcode, "G02") == 0 || strcmp(previous_gcode, "G2") == 0){
                    //cutter comp right
                    if (strcmp(cutter_comp_direction, "right") == 0) {
                        if (atof(prev_i_pos)>0) {
                            x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(-1);
                        } else if (atof(prev_i_pos)<0){
                            x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(1);
                        } else {
                            x_arc_center = prev_x_final_pos;
                        }
                        if (atof(prev_j_pos)>0) {
                            y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(-1);
                        } else if(atof(prev_i_pos)<0) {
                            y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(1);
                        } else {
                            y_arc_center = prev_y_final_pos;
                        }
                        radius = radius - d_len2/2;
                    //cutter comp left
                    } else {
                        if (atof(prev_i_pos)>0) {
                            x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(1);
                        } else if (atof(prev_i_pos)<0) {
                            x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(-1);
                        } else {
                            x_arc_center = prev_x_final_pos;
                        }
                        if (atof(prev_j_pos)>0) {
                            y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(1);
                        } else if (atof(prev_j_pos)<0) {
                            y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(-1);
                        } else {
                            y_arc_center = prev_y_final_pos;
                        }
                        radius = radius + d_len2/2;
                    }
                //counterclockwise arc
                } else {
                    //cutter comp right
                    if (strcmp(cutter_comp_direction, "right") == 0) {
                        if (atof(prev_i_pos)>0) {
                            x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(1);
                        } else if (atof(prev_i_pos)<0) {
                            x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(-1);
                        } else {
                            x_arc_center = prev_x_final_pos;
                        }
                        if (atof(prev_j_pos)>0) {
                            y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(1);
                        } else if (atof(prev_j_pos)<0) {
                            y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(-1);
                        } else {
                            y_arc_center = prev_y_final_pos;
                        }
                        radius = radius + d_len2/2;
                    //cutter comp left
                    } else {
                        if (atof(prev_i_pos)>0) {
                            x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(-1);
                        } else if (atof(prev_i_pos)<0){
                            x_arc_center = prev_x_final_pos + atof(prev_i_pos) + (d_len2/2)*(1);
                        } else {
                            x_arc_center = prev_x_final_pos;
                        }
                        if (atof(prev_j_pos)>0) {
                            y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(-1);
                        } else if (atof(prev_j_pos)<0) {
                            y_arc_center = prev_y_final_pos + atof(prev_j_pos) + (d_len2/2)*(1);
                        } else {
                            y_arc_center = prev_y_final_pos;
                        }
                        radius = radius - d_len2/2;
                    }
                }
            }

            float slope = (y_arc_center - prev_y_final_pos) / (x_arc_center - prev_x_final_pos);
            float intercept = prev_y_final_pos - slope * prev_x_final_pos;

            //calculate central angle of arc
            if(prev_x_final_pos == *x_final_pos && prev_y_final_pos == *y_final_pos) {
                central_angle = 360;
            } else if ((*y_final_pos == slope * *x_final_pos + intercept) || ((slope == -INFINITY || slope == INFINITY) && isnan(intercept))) {
                central_angle = 180;
            } else {
                //clockwise arc
                if (((strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0) && (d_len2 == 0 || isnan(d_len2))) ||
                ((strcmp(previous_gcode, "G02") == 0 || strcmp(previous_gcode, "G2") == 0) && d_len2 > 0)) {
                    //arc starts in quadrant 2
                    if ((slope < 0 && atof(prev_i_pos) > 0 && atof(prev_j_pos) < 0 && d_len2 > 0) || 
                    (slope < 0 && atof(i_pos) > 0 && atof(j_pos) < 0 && (d_len2 == 0 || isnan(d_len2))) ||
                    (slope == 0 && prev_x_final_pos < *x_final_pos)) {
                        //over 180 degree angle
                        if (*y_final_pos < slope * *x_final_pos + intercept) {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = 360 - (arc_length * 360) / (2 * M_PI * radius);
                        //less than 180 degree angle
                        } else {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = (arc_length * 360) / (2 * M_PI * radius);
                        }
                    //arc starts in quadrant 4
                    } else if ((slope < 0 && atof(prev_i_pos) < 0 && atof(prev_j_pos) > 0 && d_len2 > 0) || 
                    (slope < 0 && atof(i_pos) < 0 && atof(j_pos) > 0 && (d_len2 == 0 || isnan(d_len2))) ||
                    (slope == 0 && prev_x_final_pos > *x_final_pos)) {
                        //over 180 degree angle
                        if (*y_final_pos > slope * *x_final_pos + intercept) {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = 360 - (arc_length * 360) / (2 * M_PI * radius);
                        //less than 180 degree angle
                        } else {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = (arc_length * 360) / (2 * M_PI * radius);
                        }
                    //arc starts in quadrant 1
                    } else if ((slope > 0 && atof(prev_i_pos) < 0 && atof(prev_j_pos) < 0 && d_len2 > 0) || 
                    (slope > 0 && atof(i_pos) < 0 && atof(j_pos) < 0 && (d_len2 == 0 || isnan(d_len2))) ||
                    (slope == -INFINITY && intercept == INFINITY)) {
                        //over 180 degree angle
                        if (*y_final_pos > slope * *x_final_pos + intercept) {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = 360 - (arc_length * 360) / (2 * M_PI * radius);
                        //less than 180 degree angle
                        } else {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = (arc_length * 360) / (2 * M_PI * radius);
                        }
                    //arc starts in quadrant 3
                    } else if ((slope > 0 && atof(prev_i_pos) > 0 && atof(prev_j_pos) > 0 && d_len2 > 0) || 
                    (slope > 0 && atof(i_pos) > 0 && atof(j_pos) > 0 && (d_len2 == 0 || isnan(d_len2))) ||
                    (slope == INFINITY && intercept == -INFINITY)) {
                        //over 180 degree angle
                        if (*y_final_pos < slope * *x_final_pos + intercept) {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = 360 - (arc_length * 360) / (2 * M_PI * radius);
                        //less than 180 degree angle
                        } else {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = (arc_length * 360) / (2 * M_PI * radius);
                        }
                    }
                //counterclockwise arc
                } else {
                    //arc starts in quadrant 2
                    if ((slope < 0 && atof(prev_i_pos) > 0 && atof(prev_j_pos) < 0 && d_len2 > 0) || 
                    (slope < 0 && atof(i_pos) > 0 && atof(j_pos) < 0 && (d_len2 == 0 || isnan(d_len2))) ||
                    (slope == -INFINITY && intercept == INFINITY)) {
                        //over 180 degree angle
                        if (*y_final_pos > slope * *x_final_pos + intercept) {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = 360 - (arc_length * 360) / (2 * M_PI * radius);
                        //less than 180 degree angle
                        } else {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = (arc_length * 360) / (2 * M_PI * radius);
                        }
                    //arc starts in quadrant 4
                    } else if ((slope < 0 && atof(prev_i_pos) < 0 && atof(prev_j_pos) > 0 && d_len2 > 0) || 
                    (slope < 0 && atof(i_pos) < 0 && atof(j_pos) > 0 && (d_len2 == 0 || isnan(d_len2))) ||
                    (slope == INFINITY && intercept == -INFINITY)) {
                        //over 180 degree angle
                        if (*y_final_pos < slope * *x_final_pos + intercept) {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = 360 - (arc_length * 360) / (2 * M_PI * radius);
                        //less than 180 degree angle
                        } else {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = (arc_length * 360) / (2 * M_PI * radius);
                        }
                    //arc starts in quadrant 1
                    } else if ((slope > 0 && atof(prev_i_pos) < 0 && atof(prev_j_pos) < 0 && d_len2 > 0) || 
                    (slope > 0 && atof(i_pos) < 0 && atof(j_pos) < 0 && (d_len2 == 0 || isnan(d_len2))) ||
                    (slope == 0 && prev_x_final_pos > *x_final_pos)) {
                        //over 180 degree angle
                        if (*y_final_pos < slope * *x_final_pos + intercept) {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = 360 - (arc_length * 360) / (2 * M_PI * radius);
                        //less than 180 degree angle
                        } else {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = (arc_length * 360) / (2 * M_PI * radius);
                        }
                    //arc starts in quadrant 3
                    } else if ((slope > 0 && atof(prev_i_pos) > 0 && atof(prev_j_pos) > 0 && d_len2 > 0) || 
                    (slope > 0 && atof(i_pos) > 0 && atof(j_pos) > 0 && (d_len2 == 0 || isnan(d_len2))) ||
                    (slope == 0 && prev_x_final_pos < *x_final_pos)) {
                        //over 180 degree angle
                        if (*y_final_pos > slope * *x_final_pos + intercept) {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = 360 - (arc_length * 360) / (2 * M_PI * radius);
                        //less than 180 degree angle
                        } else {
                            float arc_length = 2*radius*asin(sqrt((*x_final_pos - prev_x_final_pos)*(*x_final_pos - prev_x_final_pos) + 
                            (*y_final_pos - prev_y_final_pos)*(*y_final_pos - prev_y_final_pos))/(2*radius));
                            central_angle = (arc_length * 360) / (2 * M_PI * radius);
                        }
                    }
                }
            }

            //print intermediate points
            while (index < 1000) {
                float temp_angle = (central_angle / 1000) * index;
                if (((strcmp(recent_gcode, "G02") == 0 || strcmp(recent_gcode, "G2") == 0) && (d_len2 == 0 || isnan(d_len2))) ||
                ((strcmp(previous_gcode, "G02") == 0 || strcmp(previous_gcode, "G2") == 0) && d_len2 > 0)) {
                    temp_angle = temp_angle * -1;
                }
                x_inter_pos = x_arc_center + (prev_x_final_pos - x_arc_center) * cos(temp_angle * (M_PI/180)) + (y_arc_center - prev_y_final_pos) * sin(temp_angle * (M_PI/180));
                y_inter_pos = y_arc_center + (prev_y_final_pos - y_arc_center) * cos(temp_angle * (M_PI/180)) + (prev_x_final_pos - x_arc_center) * sin(temp_angle * (M_PI/180));
                if (strcmp(prev_z_pos,"n/a") == 0) {
                    z_inter_pos = 0;
                } else {
                    z_inter_pos = atof(prev_z_pos);
                }
                printf("| X Position | Y Position | Z Position | Current Tool | Spindle Speed |\n");
                fprintf(fp, "| X Position | Y Position | Z Position | Current Tool | Spindle Speed |\n");
                printf("| %.3f | %.3f | %.3f | %s | %s |\n", x_inter_pos, y_inter_pos, z_inter_pos, curr_tool, spindle_speed);
                fprintf(fp, "| %.3f | %.3f | %.3f | %s | %s |\n", x_inter_pos, y_inter_pos, z_inter_pos, curr_tool, spindle_speed);
                index = index + 1;
            }
        }
    }
    ////final print format//////
    printf("| X Position | Y Position | Z Position | Current Tool | Spindle Speed |\n");
    fprintf(fp, "| X Position | Y Position | Z Position | Current Tool | Spindle Speed |\n");
    if (strcmp(z_pos,"n/a") == 0) {
        printf("| %.3f | %.3f | %.3f | %s | %s |\n", *x_final_pos, *y_final_pos, NAN, curr_tool, spindle_speed);
        fprintf(fp, "| %.3f | %.3f | %.3f | %s | %s |\n", *x_final_pos, *y_final_pos, NAN, curr_tool, spindle_speed);
    } else {
        printf("| %.3f | %.3f | %.3f | %s | %s |\n", *x_final_pos, *y_final_pos, atof(z_pos), curr_tool, spindle_speed);
        fprintf(fp, "| %.3f | %.3f | %.3f | %s | %s |\n", *x_final_pos, *y_final_pos, atof(z_pos), curr_tool, spindle_speed);
    }
    if (strcmp(recent_gcode, "G40") == 0) {
        d_len2 = 0;
    }
}