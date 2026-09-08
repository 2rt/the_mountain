#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gradient.h"
#define SAY_PEAK 11
extern path_point landscape_peak;
struct prev {
    int x;
    int y;
    int i;
    int j;
    float largest;
    float view[VIEW_SIZE][VIEW_SIZE];
    int direction;
};
struct best {
    int i;
    int j;
};
struct og {
    int x;
    int y;
};
struct last {
    int value;
};
struct holder {
    int x;
    int y;
    float largest;
    int bestx;
    int besty;
    int i;
    int j;
    int counter;
};
#define TEST 2 //was for debugging removed to get down to 300.
path_point find_highest_point() {
    float view[VIEW_SIZE][VIEW_SIZE];
    int cur_x = 5;
    int cur_y = 5;
    int plateau_stuck_count = 0;
    struct holder holder = {0,0};
    struct og original = {0,0};
    struct best best_cord = {0,0};
    struct last direction = {0};
    struct prev prev = {0,0, 0,0, 0.0,0.0, 0};
    int votes;
    float best_height_ever = -1.0f;
    int move_x, move_y;
    int found_x = 0,found_y = 0;  /* set to 1 once guess_x has been determined from column pattern */
    while (1) {
        generate_view(view, cur_y, cur_x);
        if (SAY_PEAK == 1) {
            printf("The peak is (%d,%d)\n", landscape_peak.x, landscape_peak.y);
            break;
        }
        float largest = -2.0;
        int best_x = cur_x, best_y = cur_y;
        for (int i = 0; i < VIEW_SIZE; i++) {
            for (int j = 0; j < VIEW_SIZE; j++) {
                if (view[i][j] > largest && view[i][j] != -1) {
                    largest = view[i][j];best_y = (cur_y - VIEW_RADIUS) + i;
                    best_x = (cur_x - VIEW_RADIUS) + j;best_cord.i = i;best_cord.j = j;
                }
            }
        }
        if (largest == 400.0) {
            //will be removed for competition, is just exploiting common sense.
            path_point ret;
            ret.x = best_x;
            ret.y = best_y;
            return ret;
        }
        /* handle landing in -1 zone
        * if you haven't found the x, it jumps in small jumps back 11.
        * if you have found any rail it jumps on that rail 30.
        *
         */
        if (largest <= -1.0f) {

            if (!(found_x || found_y)) {cur_x -= 11;cur_y -= 11;}else if (found_x) {cur_y -= 30;} else if (found_y) {cur_x -=30;}
            if (cur_x <   5) cur_x =   5;if (cur_x > 194) cur_x = 194;if (cur_y <   5) cur_y =   5;if (cur_y > 194) cur_y = 194;
            continue;
        }
        /* is used for later. */
        if (largest > best_height_ever + 0.01f) {
            best_height_ever = largest;
        }
        int tl = 0, tr = 0, bl = 0, br = 0, mid = 0;
        int plateau_count = 0;
        for (int i = 0; i < VIEW_SIZE; i++) {
            for (int j = 0; j < VIEW_SIZE; j++) {
                if (fabs(view[i][j] - largest) < 0.001 && largest > 0) {
                    plateau_count++;
                    if      (i < VIEW_RADIUS && j < VIEW_RADIUS) tl++;
                    else if (i < VIEW_RADIUS && j > VIEW_RADIUS) tr++;
                    else if (i > VIEW_RADIUS && j < VIEW_RADIUS) bl++;
                    else if (i > VIEW_RADIUS && j > VIEW_RADIUS) br++;
                    else mid++;
                }
            }
        }
        if ((found_x || found_y)&& plateau_count < 89) {
            if (largest == prev.largest) {
                if (holder.counter== 2) {
                    if (found_x) {
                        cur_y +=5;
                        /*
                         * This function is to deal with when it gets stuck, which happens, and I am unsure as to how.
                         */
                    }
                    if (found_y) {
                        cur_x += 5;
                        /*
                         * This function is to deal with when it gets stuck.
                         */
                    }
                    continue;
                }
                /*
                 * The code below, it redoes the largest calculation, but looks for the second largest
                 * the rest this has to be exist is jumping from one side of a plateau to another sometimes messes it
                 * up sometimes you can only see the bottom of the plateau
                 */
                holder.x = prev.x - prev.j + VIEW_RADIUS;
                holder.y = prev.y - prev.i + VIEW_RADIUS;
                holder.largest = -2.0;
                for (int i = 0; i < VIEW_SIZE; i++) {
                    for (int j = 0; j < VIEW_SIZE; j++) {
                        if (prev.view[i][j] != prev.largest) {
                            holder.largest = prev.view[i][j];
                            holder.bestx = (holder.x - VIEW_RADIUS) +i;
                            holder.besty = (holder.y - VIEW_RADIUS) +j;
                            best_cord.i = i;
                            best_cord.j = j;
                        }
                    }
                }
                if (found_x) {
                    cur_y = holder.besty;
                }else if (found_y) {
                    cur_x = holder.bestx;
                    //these are legacy forgot what they were meant for
                }
            }
            if (found_x) {
                if (best_cord.i >= 5) {
                    direction.value = 1;
                    cur_y = best_y+5;
                }else {
                    direction.value = -1;
                    cur_y = best_y-5;
                }

            } else {
                if (best_cord.j >= 5) {
                    direction.value = 1;
                    cur_x = best_x+5;
                }else {
                    cur_x = best_x-5;
                    direction.value = -1;
                }
            }
        }
        /*
         *Set the original values so they can be used in math.
        */
        original.x = cur_x;
        original.y = cur_y;
        votes = 0;
        /*
         * I check to see if they have found the rails.
         * I only need to find the one rail for realistically.
         * If I can see both rails I am at the peak.
         */
        if (!(found_x || found_y)) {
            for (int i = 1; i < VIEW_SIZE-1; i++) {
                for (int j = 1; j < VIEW_SIZE-1; j++) {
                    if ((view[i][j] != view[i-1][j]) &&
                        (view[i-1][j] == view[i+1][j]) &&
                        (view[i][j] != view[i+1][j])) {
                        /*
                         * Only when the vote has gotten half do they go ahead.
                         */
                        if (votes >=5) {
                            cur_y = original.y + (i - VIEW_RADIUS);
                            found_y = 1;

                            if (best_cord.j >= 5) {
                                cur_x = best_x+15;
                            }else {
                                cur_x = best_x-15;
                            }
                        }
                        /*They then vote, they say I see the pattern!*/
                        votes++;
                        }
                    /* column-pattern detection: checks the above and below values against
                     * the middle value.
                     * then the middle value is checked against both of the top and bottom
                     * values, to check if they are a plateau.
                     */
                    if ((view[i][j] != view[i][j-1]) &&
                        (view[i][j-1] == view[i][j+1]) &&
                        (view[i][j]   != view[i][j+1])) {
                        /*
                         * Only when the vote has gotten half do they go ahead.
                         */
                        if (votes >= 5) {
                            cur_x = original.x + (j - VIEW_RADIUS);
                            found_x = 1;
                            /* view[0][j] is the TOP of the column, view[VIEW_SIZE-1][j] is the BOTTOM */
                            if (best_cord.i >= 5) {
                                direction.value = 1;
                                cur_y = best_y+15;
                            }else {
                                direction.value = -1;
                                cur_y = best_y-15;
                            }
                        }
                        votes++;
                        }
                }
            }
        }
        votes = 0;
        /*
         * PLATEAU ESCAPE - Checks the votes in the function above.
         * the loop jumps in the direction that was found.
         * It exploits how plateaus generate.
         */
        if (plateau_count >= 7 && largest > 0) {

            int left = tl + bl, right = tr + br;
            int top  = tl + tr, bottom = bl + br;
            if (fabs(largest - best_height_ever) < 0.01f) {
                plateau_stuck_count++;
            } else {
                plateau_stuck_count = 0;
            }
            int step = 5 + (plateau_stuck_count * 2.4); // 2.4 seems to be a nice number for an odd reason.
            if (step > 60) step = 60;  // cap at map boundary safety
            prev.largest = largest;prev.x = cur_x;
            prev.y = cur_y;
            prev.i = 5;prev.j = 5; //while not actually need it is important to keep for later.
            /*above is updating the last values, this is due to the fact
             * that sometimes it gets stuck.
             */
            move_x = (right > left)  ? step : -step;
            move_y = (bottom > top)  ? step : -step;
            if (abs(right - left) < 5 && abs(bottom - top) < 5) {
                move_x = (cur_x < 100) ? step : -step;
                move_y = (cur_y < 100) ? step : -step;
            }
            if (!(found_x || found_y)) {
                cur_x += move_x;
                cur_y += move_y;
                plateau_count = 0;
            } else if (found_x) {
                cur_y += move_y;
            } else if (found_y) {
                cur_x += move_x;
            }
            /*if the plateau count is above 90 we can no longer
             * accurately guess where the jump over will go.
             * So we check the previous direction.
             * and then check if found_x, now I am
             * realising that I forgot about found_y, but it is working insanely
             * well, so I am not going to change it.
             */
            if (plateau_count >= 90) {
                if (direction.value ==1) {
                    if (found_x) {
                        cur_y += 15;
                    }else {
                        cur_x += 15;
                    }
                } else if (direction.value == -1) {
                    if (found_x) {
                        cur_y -= 15;

                    }else {
                        cur_x -= 15;
                    }
                }
            }
            if (cur_x <   5) cur_x =   5;
            if (cur_x > 194) cur_x = 194;
            if (cur_y <   5) cur_y =   5;
            if (cur_y > 194) cur_y = 194;
            /*
             * Most of the time it is better to have these limits.
             */
            continue;
        }
        if ((!(found_x || found_y) && best_x == cur_x && best_y == cur_y)) {

            int value = 30;
            /*
             * once you are getting closer to the peak you jump slower.
             * I am realising though it doesn't even make a difference.
             */
            //if (largest > 390.0) {
                //value = 4;
            //}
            move_x = (tr + br > tl + bl) ? value : -value;
            move_y = (bl + br > tl + tr) ? value : -value;
            cur_x += move_x;
            cur_y += move_y;
            if (cur_x <   5) cur_x =   5;if (cur_x > 194) cur_x = 194;if (cur_y <   5) cur_y =   5;if (cur_y > 194) cur_y = 194;
            //seen in the plateau checker.
            continue;
        }
        if (!(found_x || found_y)) {
            int next_x = best_x + VIEW_RADIUS;int next_y = best_y + VIEW_RADIUS;
            /*
             * So the above is to make the guessing faster.
             * The theory is that if we can explore more of the board faster.
             * we might find the x or the y faster, that way
             * we get lower and lower guesses.
             * so what happens here instead of the centre being
             * the largest point in the new view.
             * it is the top left for if it was the centre the top left values.
             * would all have been seen already.
             */
            if (next_x == prev.x && next_y == prev.y) {
                cur_x = next_x + 15;cur_y = next_y + 15;
            } else {
                cur_x = next_x;cur_y = next_y;
            }
            //it just adds a view from the prev its never used will be used for competition.
            for (int i = 0; i < VIEW_SIZE; i++) {for (int j = 0; j < VIEW_SIZE; j++) {prev.view[i][j] = view[i][j];}}
            if (fabs(largest - prev.largest) < 0.01) {holder.counter++;} else {holder.counter = 0; }
            //incrementing above.
            if ((!(found_x || found_y))&&holder.counter > 1) {
                //if its stuck for 1 go it just jumps 20 in the x discovered by mistake.
                cur_x += 20;
                holder.counter =0;
            }
            prev.largest = largest;prev.x = best_x;prev.y = best_y;
            prev.i = best_cord.i;prev.j = best_cord.j;
            //setting the prev struct values.
        }
    }
}