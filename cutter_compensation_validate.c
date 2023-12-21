#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <ctype.h>
#define BUFFERSIZE (256)
#include "num_check.c"

//calcuates and applies cutter compensation
//cutter_comp_direction = which direction the cutter compensation is applied to "left","right",or "n/a"
//comp_count = the number of lines that have compensation applied
//d_len2 = the diameter of the tool
//x_comp_pos = the compensation position of the X coordinate
//y_comp_pos = the compensation position of the Y coordinate
int cutter_compensation_validate(char *cutter_comp_direction, int comp_count,
    char *x_pos, char *y_pos, float *d_len2, char *prev_x_pos, char *prev_y_pos,
    float *x_comp_pos, float *y_comp_pos, char *recent_gcode, char *prev_x_pos2, char *prev_y_pos2,
    char *previous_gcode) {
    if (comp_count > 0){
        //for first compensation move on x axis
        if (strcmp(prev_x_pos, x_pos) == 0 && strcmp(prev_y_pos, y_pos) != 0 && isnan(*x_comp_pos)) {
            if (strcmp(cutter_comp_direction, "left") == 0) {
                *x_comp_pos = atof(prev_x_pos) - (*d_len2/2);
            } else if (strcmp(cutter_comp_direction, "right") == 0) {
                *x_comp_pos = atof(prev_x_pos) + (*d_len2/2);
            }
            *y_comp_pos = atof(prev_y_pos);
        //for first compensation move on y axis
        } else if (strcmp(prev_x_pos, x_pos) != 0 && strcmp(prev_y_pos, y_pos) == 0 && isnan(*y_comp_pos)) {
            *y_comp_pos = atof(prev_y_pos) + (*d_len2/2);
            *x_comp_pos = atof(prev_x_pos);
        //360 degree circle
        } else if (strcmp(prev_x_pos, x_pos) == 0 && strcmp(prev_y_pos, y_pos) == 0 && 
            (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0 || 
            strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0)) {
            //clockwise circle
            if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                //cutter compensation left
                if (strcmp(cutter_comp_direction, "left") == 0) {
                    *x_comp_pos = atof(prev_x_pos);
                    *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                //cutter compensation right
                } else {
                    *x_comp_pos = atof(prev_x_pos);
                    *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                }
            //counterclockwise circle
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                //cutter compensation left
                if (strcmp(cutter_comp_direction, "left") == 0) {
                    *x_comp_pos = atof(prev_x_pos);
                    *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                //cutter compensation right
                } else {
                    *x_comp_pos = atof(prev_x_pos);
                    *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                }
            }
        //for point at bottom of slant/arc move to top right (clockwise and counterclockwise)
        //       xxxxxxx
        //            xx
        //        xx  x
        //     xxx    x
        //   xxx
        //  xx
        // x
        } else if (atof(x_pos) > atof(prev_x_pos) && atof(y_pos) > atof(prev_y_pos)) {
            //for clockwise arc
            if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                *y_comp_pos = atof(prev_y_pos);
            //for counterclockwise arc
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
            //for straight lines
            } else {
                float helperA = atof(x_pos) - atof(prev_x_pos);
                float helperB = atof(y_pos) - atof(prev_y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = NAN;

                if (strcmp(cutter_comp_direction, "right") == 0) {
                    //non-acute angle corner
                    if (atof(prev_x_pos) >= atof(prev_x_pos2)) {
                        helperAngle = asin(helperA/helperC) * (180/M_PI);
                    //acute angle corner
                    } else {
                        helperAngle = asin(helperB/helperC) * (180/M_PI);
                        float bigArea = (helperA * helperB) / 2;
                        float hyp = (*d_len2/2) / sin((helperAngle/2) * (M_PI/180));
                        float smallArea = ((*d_len2/2) * sqrt(hyp * hyp - (*d_len2/2) * (*d_len2/2))) / 2;
                        //too small of angle corner
                        if ((bigArea-smallArea)/bigArea < .95) {
                            return 0;
                        }
                    }
                //cutter compensation left
                } else {
                    //non-acute angle corner
                    if (atof(prev_y_pos) >= atof(prev_y_pos2)) {
                        helperAngle = asin(helperB/helperC) * (180/M_PI);
                    //acute angle corner
                    } else {
                        helperAngle = asin(helperA/helperC) * (180/M_PI);
                        float bigArea = (helperA * helperB) / 2;
                        float hyp = (*d_len2/2) / sin((helperAngle/2) * (M_PI/180));
                        float smallArea = ((*d_len2/2) * sqrt(hyp * hyp - (*d_len2/2) * (*d_len2/2))) / 2;
                        //too small of angle corner
                        if ((bigArea-smallArea)/bigArea < .95) {
                            return 0;
                        }
                    }
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (*d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (*d_len2/2) * (*d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    //for obtuse exterior angle
                    if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    //for acute exterior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_x_pos2) > atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    //for obtuse interior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    //for acute interior angle
                    } else {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    }
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    //for obtuse interior angle
                    if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    //for acute interior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_x_pos2) > atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    //for obtuse exterior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_x_pos2) < atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    //for acute exterior angle
                    } else {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    }
                }
            }
        //for cut going from left to right
        //              xx
        //                xx
        //                  xxx
        // xxxxxxxxx xxxxx    x
        //                   xx
        //                xxx
        //              xx
        } else if (atof(x_pos) > atof(prev_x_pos) && atof(y_pos) == atof(prev_y_pos)) {
            //for clockwise arc
            if (strcmp(previous_gcode, "G2") == 0 || strcmp(previous_gcode, "G02") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
            //for counterclockwise arc
            } else if (strcmp(previous_gcode, "G3") == 0 || strcmp(previous_gcode, "G03") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
            //for straight lines
            } else {
                float helperA = atof(prev_x_pos) - atof(prev_x_pos2);
                float helperB = atof(prev_y_pos) - atof(prev_y_pos2);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = asin(helperA/helperC) * (180/ M_PI);

                if (atof(prev_x_pos2) > atof(prev_x_pos) && atof(prev_y_pos2) > atof(prev_y_pos)) {
                    helperAngle = asin(helperB/helperC) * (180/M_PI);
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (*d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (*d_len2/2) * (*d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    //for exterior angles
                    if (atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    //for interior angles
                    } else {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    }
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    //for exterior angles
                    if (atof(prev_y_pos2) > atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    //for interior angles
                    } else {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    }
                }
            }
        //for point at top of slant/arc move to bottom right (clockwise and counterclockwise)
        // x
        //  xx
        //   xx
        //    xx
        //     xx  x
        //   xx xx x
        //     xx  x
        //        xx 
        } else if (atof(x_pos) > atof(prev_x_pos) && atof(y_pos) < atof(prev_y_pos)) {
            //for clockwise arc
            if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
            //for counterclockwise arc
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                *y_comp_pos = atof(prev_y_pos);
            //for sraight lines
            } else {
                float helperA = atof(x_pos) - atof(prev_x_pos);
                float helperB = atof(prev_y_pos) - atof(y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = NAN; 

                if (strcmp(cutter_comp_direction, "right") == 0) {
                    //non-acute angle corner
                    if (atof(prev_y_pos) <= atof(prev_y_pos2)) {
                        helperAngle = asin(helperB/helperC) * (180/M_PI);
                    //acute angle corner
                    } else {
                        helperAngle = asin(helperA/helperC) * (180/M_PI);
                        float bigArea = (helperA * helperB) / 2;
                        float hyp = (*d_len2/2) / sin((helperAngle/2) * (M_PI/180));
                        float smallArea = ((*d_len2/2) * sqrt(hyp * hyp - (*d_len2/2) * (*d_len2/2))) / 2;
                        //too small of angle corner check
                        if ((bigArea-smallArea)/bigArea < .95) {
                            return 0;
                        }
                    }
                //cutter compensation left
                } else {
                    //non-acute angle corner
                    if (atof(prev_x_pos) >= atof(prev_x_pos2)) {
                        helperAngle = asin(helperA/helperC) * (180/M_PI);
                    //acute angle corner
                    } else {
                        helperAngle = asin(helperB/helperC) * (180/M_PI);
                        float bigArea = (helperA * helperB) / 2;
                        float hyp = (*d_len2/2) / sin((helperAngle/2) * (M_PI/180));
                        float smallArea = ((*d_len2/2) * sqrt(hyp * hyp - (*d_len2/2) * (*d_len2/2))) / 2;
                        //too small of angle corner check
                        if ((bigArea-smallArea)/bigArea < .95) {
                            return 0;
                        }
                    }
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (*d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (*d_len2/2) * (*d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    //for acute exterior angle
                    if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    //for obtuse exterior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_x_pos2) < atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    //for obtuse interior angle
                    } else if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) > atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    //for acute interior angle
                    } else {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    }
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    //for acute interior angle
                    if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    //for obtuse interior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_x_pos2) < atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    //for obtuse exterior angle
                    } else if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) > atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    //for acute exterior angle
                    } else {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    }
                }
            }
        //for cut going from top to bottom
        //     x
        //     x
        //     x
        //     x
        // x   x   x
        // x   x   x
        // xx  x  xx
        //  xx    x
        //   xxxxxx
        //     xx
        } else if (atof(x_pos) == atof(prev_x_pos) && atof(y_pos) < atof(prev_y_pos)) {
            //for clockwise arc
            if (strcmp(previous_gcode, "G2") == 0 || strcmp(previous_gcode, "G02") == 0) {
                *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                *y_comp_pos = atof(prev_y_pos);
            //for counterclockwise arc
            } else if (strcmp(previous_gcode, "G3") == 0 || strcmp(previous_gcode, "G03") == 0) {
                //if executing arc into arc move
                if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                    *x_comp_pos = atof(prev_x_pos);
                    *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                } else  {
                    *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                    *y_comp_pos = atof(prev_y_pos);
                }
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
            //for straight lines
            } else {
                float helperA = atof(prev_x_pos) - atof(prev_x_pos2);
                float helperB = atof(prev_y_pos2) - atof(prev_y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngleB = asin(helperB/helperC) * (180/M_PI);
                float extraAngle = 90 - fabs(helperAngleB);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (*d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (*d_len2/2) * (*d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    //for exterior angles
                    if (atof(prev_x_pos2) < atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    //for interior angles
                    } else {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    }
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    //for exterior angles
                    if (atof(prev_x_pos2) > atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    //for interior angles
                    } else {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    }
                }
            }
        //for point at top of slant/arc move to bottom left (clockwise and counterclockwise)
        //             xx
        // x        xxx
        // x     xxxx
        // x   xx
        // x xxx
        // xxxxxxxxxx
        } else if (atof(x_pos) < atof(prev_x_pos) && atof(y_pos) < atof(prev_y_pos)) {
            //for clockwise arc
            if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                *y_comp_pos = atof(prev_y_pos);
            //for counterclockwise arc
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
            //for straight lines
            } else {
                float helperA = atof(prev_x_pos) - atof(x_pos);
                float helperB = atof(prev_y_pos) - atof(y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = NAN; 

                if (strcmp(cutter_comp_direction, "right") == 0) {
                    //non-acute angle corner
                    if (atof(prev_x_pos) <= atof(prev_x_pos2)) {
                        helperAngle = asin(helperA/helperC) * (180/M_PI);
                    //acute angle corner
                    } else {
                        helperAngle = asin(helperB/helperC) * (180/M_PI);
                        float bigArea = (helperA * helperB) / 2;
                        float hyp = (*d_len2/2) / sin((helperAngle/2) * (M_PI/180));
                        float smallArea = ((*d_len2/2) * sqrt(hyp * hyp - (*d_len2/2) * (*d_len2/2))) / 2;
                        //too small acute angle check
                        if ((bigArea-smallArea)/bigArea < .95) {
                            return 0;
                        }
                    }
                //cutter compensation left
                } else {
                    //non-acute angle corner
                    if (atof(prev_y_pos) <= atof(prev_y_pos2)) {
                        helperAngle = asin(helperB/helperC) * (180/M_PI);
                    //acute angle corner
                    } else {
                        helperAngle = asin(helperA/helperC) * (180/M_PI);
                        float bigArea = (helperA * helperB) / 2;
                        float hyp = (*d_len2/2) / sin((helperAngle/2) * (M_PI/180));
                        float smallArea = ((*d_len2/2) * sqrt(hyp * hyp - (*d_len2/2) * (*d_len2/2))) / 2;
                        //too small acute angle check
                        if ((bigArea-smallArea)/bigArea < .95) {
                            return 0;
                        }
                    }
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (*d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (*d_len2/2) * (*d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    //for acute interior angle
                    if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    //for obtuse interior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_x_pos2) > atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    //for obtuse exterior angle
                    } else if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) > atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    //for acute exterior angle
                    } else {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    }
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    //for acute exterior angle
                    if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    //for obtuse exterior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_x_pos2) > atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    //for obtuse interior angle
                    } else if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) > atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    //for acute interior angle
                    } else {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    }
                }
            }
        //for cut from right to left
        //       x
        //    xxxx
        //  xxx
        // xx   xxx xxxx x xxxxxx
        //  xxx
        //    xxx
        //       x
        } else if (atof(x_pos) < atof(prev_x_pos) && atof(y_pos) == atof(prev_y_pos)) {
            //for clockwise arc
            if (strcmp(previous_gcode, "G2") == 0 || strcmp(previous_gcode, "G02") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                if (strcmp(cutter_comp_direction, "right") == 0) {
                    *x_comp_pos = atof(prev_x_pos);
                    *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                }
            //for counterclockwise arc
            } else if (strcmp(previous_gcode, "G3") == 0 || strcmp(previous_gcode, "G03") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
            //for straight lines
            } else {
                float helperA = atof(prev_x_pos2) - atof(prev_x_pos);
                float helperB = atof(prev_y_pos2) - atof(prev_y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngleA = asin(helperA/helperC) * (180/M_PI);
                float extraAngle = 90 - fabs(helperAngleA);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (*d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (*d_len2/2) * (*d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    *x_comp_pos = atof(prev_x_pos) + comp;
                    *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    *x_comp_pos = atof(prev_x_pos) + comp;
                    *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                }

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    //for exterior angles
                    if (atof(prev_y_pos2) > atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    //for interior angles
                    } else {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    }
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    //for exterior angles
                    if (atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    //for interior angles
                    } else {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    }
                }
            }
        //for point at bottom of slant/arc move to top left (clockwise and counterclockwise)
        // xxxxxxxxx
        // x  x
        // x   xxx
        // x     xxx
        // x       xxx
        //            xx
        //             xxx
        } else if (atof(x_pos) < atof(prev_x_pos) && atof(y_pos) > atof(prev_y_pos)) {
            //for clockwise arc
            if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
            //for counterclockwise arc
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                *y_comp_pos = atof(prev_y_pos);
            //for straight lines
            } else {
                float helperA = atof(prev_x_pos) - atof(x_pos);
                float helperB = atof(y_pos) - atof(prev_y_pos);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngle = NAN;

                if (strcmp(cutter_comp_direction, "right") == 0) {
                    //non-acute angle corner
                    if (atof(prev_y_pos) >= atof(prev_y_pos2)) {
                        helperAngle = asin(helperB/helperC) * (180/M_PI);
                    //acute angle corner
                    } else {
                        helperAngle = asin(helperA/helperC) * (180/M_PI);
                        float bigArea = (helperA * helperB) / 2;
                        float hyp = (*d_len2/2) / sin((helperAngle/2) * (M_PI/180));
                        float smallArea = ((*d_len2/2) * sqrt(hyp * hyp - (*d_len2/2) * (*d_len2/2))) / 2;
                        //too small of acute angle check
                        if ((bigArea-smallArea)/bigArea < .95) {
                            return 0;
                        }
                    }
                //cutter compensation left side
                } else {
                    //non-acute angle corner
                    if (atof(prev_x_pos) <= atof(prev_x_pos2)) {
                        helperAngle = asin(helperA/helperC) * (180/M_PI);
                    //acute angle corner
                    } else {
                        helperAngle = asin(helperB/helperC) * (180/M_PI);
                        float bigArea = (helperA * helperB) / 2;
                        float hyp = (*d_len2/2) / sin((helperAngle/2) * (M_PI/180));
                        float smallArea = ((*d_len2/2) * sqrt(hyp * hyp - (*d_len2/2) * (*d_len2/2))) / 2;
                        //too small of acute angle check
                        if ((bigArea-smallArea)/bigArea < .95) {
                            return 0;
                        }
                    }
                }

                float extraAngle = 90 - fabs(helperAngle);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (*d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (*d_len2/2) * (*d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    //for acute exterior angle
                    if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) > atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    //for obtuse exterior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_x_pos2) > atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    //for obtuse interior angle
                    } else if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    //for acute interior angle
                    } else {
                        *x_comp_pos = atof(prev_x_pos) - comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    }
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    //for acute interior angle
                    if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) > atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    //for obtuse interior angle
                    } else if (strcmp(prev_y_pos2, prev_y_pos) == 0 && atof(prev_x_pos2) > atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
                    //for obtuse exterior angle
                    } else if (strcmp(prev_x_pos2, prev_x_pos) == 0 && atof(prev_y_pos2) < atof(prev_y_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    //for acute exterior angle
                    } else {
                        *x_comp_pos = atof(prev_x_pos) + comp;
                        *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                    }
                }
            }
        //for cut from bottom to top
        //     xx
        //   xx xx
        //  xx x xx
        //  x  x  xx
        // x   x   x
        //     x
        //     x
        //     x
        //     x
        //     x
        } else if (atof(x_pos) == atof(prev_x_pos) && atof(y_pos) > atof(prev_y_pos)) {
            //for clockwise arc
            if (strcmp(previous_gcode, "G2") == 0 || strcmp(previous_gcode, "G02") == 0) {
                *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                *y_comp_pos = atof(prev_y_pos);
            //for counterclockwise arc
            } else if (strcmp(previous_gcode, "G3") == 0 || strcmp(previous_gcode, "G03") == 0) {
                //if executing arc into arc move
                if (strcmp(recent_gcode, "G2") == 0 || strcmp(recent_gcode, "G02") == 0) {
                    *x_comp_pos = atof(prev_x_pos);
                    *y_comp_pos = atof(prev_y_pos) - *d_len2/2;
                } else  {
                    *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                    *y_comp_pos = atof(prev_y_pos);
                }
            } else if (strcmp(recent_gcode, "G3") == 0 || strcmp(recent_gcode, "G03") == 0) {
                *x_comp_pos = atof(prev_x_pos);
                *y_comp_pos = atof(prev_y_pos) + *d_len2/2;
            //for straight lines
            } else {
                float helperA = atof(prev_x_pos2) - atof(prev_x_pos);
                float helperB = atof(prev_y_pos) - atof(prev_y_pos2);
                float helperC = sqrt(helperA * helperA + helperB * helperB);
                float helperAngleB = asin(helperB/helperC) * (180/M_PI);
                float extraAngle = 90 - fabs(helperAngleB);
                float wholeAngle = 180 - extraAngle;
                float innAngle = wholeAngle / 2;
                float compTriangleC = (*d_len2/2) / sin(innAngle * (M_PI/180));
                float comp = sqrt(compTriangleC * compTriangleC - (*d_len2/2) * (*d_len2/2));

                if (strcmp(cutter_comp_direction, "left") == 0) {
                    //for exterior angles
                    if (atof(prev_x_pos2) > atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    //for interior angles
                    } else {
                        *x_comp_pos = atof(prev_x_pos) - *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    }
                } else if (strcmp(cutter_comp_direction, "right") == 0) {
                    //for exterior angles
                    if (atof(prev_x_pos2) < atof(prev_x_pos)) {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) - comp;
                    //for interior angles
                    } else {
                        *x_comp_pos = atof(prev_x_pos) + *d_len2/2;
                        *y_comp_pos = atof(prev_y_pos) + comp;
                    }
                }
            }
        }
        return 1;
    //If next G code is G40 
    } else if (comp_count == -1) {
        float x_comp = (*d_len2/2) * sin(45 * (M_PI/180));
        float y_comp = sqrt(((*d_len2/2) * (*d_len2/2)) - x_comp * x_comp); 
        //for point at bottom of slant/arc move to bottom left or coming from horizontal line (clockwise)
        if (atof(prev_x_pos) < atof(prev_x_pos2) && atof(prev_y_pos) <= atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "left") == 0) {
            *x_comp_pos = atof(prev_x_pos) + x_comp;
            *y_comp_pos = atof(prev_y_pos) - y_comp;
        //for point at top of slant/arc move to top left or coming from vertical line (clockwise)
        } else if (atof(prev_x_pos) <= atof(prev_x_pos2) && atof(prev_y_pos) > atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "left") == 0) {
            *x_comp_pos = atof(prev_x_pos) - x_comp;
            *y_comp_pos = atof(prev_y_pos) - y_comp;
        //for point at top of slant/arc move to top right or coming from horizontal line (clockwise)
        } else if (atof(prev_x_pos) > atof(prev_x_pos2) && atof(prev_y_pos) >= atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "left") == 0) {
            *x_comp_pos = atof(prev_x_pos) - x_comp;
            *y_comp_pos = atof(prev_y_pos) + y_comp;
        //for point at bottom of slant/arc move to bottom right or coming from vertical line (clockwise)
        } else if (atof(prev_x_pos) >= atof(prev_x_pos2) && atof(prev_y_pos) < atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "left") == 0) {
            *x_comp_pos = atof(prev_x_pos) + x_comp;
            *y_comp_pos = atof(prev_y_pos) + y_comp;
        //for point at top of slant/arc move to top left or coming from horizontal line (counterclockwise)
        } else if (atof(prev_x_pos) < atof(prev_x_pos2) && atof(prev_y_pos) >= atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "right") == 0) {
            *x_comp_pos = atof(prev_x_pos) + x_comp;
            *y_comp_pos = atof(prev_y_pos) + y_comp;
        //for point at bottom of slant/arc move to bottom left or coming from vertical line (counterclockwise)
        } else if (atof(prev_x_pos) <= atof(prev_x_pos2) && atof(prev_y_pos) < atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "right") == 0) {
            *x_comp_pos = atof(prev_x_pos) - x_comp;
            *y_comp_pos = atof(prev_y_pos) + y_comp;
        //for point at bottom of slant/arc move to bottom right or coming from horizontal line (counterclockwise)
        } else if (atof(prev_x_pos) > atof(prev_x_pos2) && atof(prev_y_pos) <= atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "right") == 0) {
            *x_comp_pos = atof(prev_x_pos) - x_comp;
            *y_comp_pos = atof(prev_y_pos) - y_comp;
        //for point at top of slant/arc move to top right or coming from vertical line (counterclockwise)
        } else if (atof(prev_x_pos) >= atof(prev_x_pos2) && atof(prev_y_pos) > atof(prev_y_pos2) && 
            strcmp(cutter_comp_direction, "right") == 0) {
            *x_comp_pos = atof(prev_x_pos) + x_comp;
            *y_comp_pos = atof(prev_y_pos) - y_comp;
        }
        return 1;
    } else if (comp_count == 0) {
        return 1;
    } else {
        return 0;
    }
}