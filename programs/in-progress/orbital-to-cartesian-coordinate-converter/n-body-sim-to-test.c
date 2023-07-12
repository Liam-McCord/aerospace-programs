#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// Made by Liam McCord
// Programmer's note - I do not know why this is such an increase in speed from the python version, but I'm not complaining. Sorry for the messy code, this is my first C program so I will likely optimize this going forwards.

// This dictates the maximum line length
#define MAX_LINE_LENGTH 1000000
// Columns in the CSV file
#define NUM_COLUMNS 9

int main() {
    // Loads the output CSV file to reset it whenever the program is re-run.
    FILE *fp2;
    fp2 = fopen("output.csv", "w");
    fprintf(fp2, "");
    fclose(fp2);

    // Opens the template planet stat CSV, this is to get some initial starting bodies and conditions.
    FILE *fp = fopen("PlanetStartStats.csv", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // Determine the number of rows in the CSV file
    int num_rows = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        num_rows++;
    }

    // Allocate memory for the 2D float array
    float **data = (float **) malloc(num_rows * sizeof(float *));
    for (int i = 0; i < num_rows; i++) {
        data[i] = (float *) malloc(NUM_COLUMNS * sizeof(float));
    }

    // Reset the file pointer to the beginning of the file
    fseek(fp, 0, SEEK_SET);

    // Read the contents of the CSV file line by line and store in the 2D array
    int row = 0;
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        char *field = strtok(line, ",");
        int col = 0;
        while (field != NULL) {
            data[row][col] = atof(field);
            field = strtok(NULL, ",");
            col++;
        }
        row++;
    }

    // Print the contents of the 2D array (Not entirely needed but used for checking data and debugging)
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < NUM_COLUMNS; j++) {
            printf("%f ", data[i][j]);
        }
        printf("\n");
    }
    
    // this sets the length of the simulation before the loop. The step number defines the total seconds before the program loops and appends to the updated CSV file. The frequency defines how many seconds between steps there are, and the step amount is the final resultant run steps.
    float step_num = 1000;
    float step_freq = 1;
    int amount_of_loops = 100;
    int step_amount = step_num / step_freq;

    printf("\n\n\n");
    int num_bodies = num_rows;
    float ts_pos[num_bodies][3][1000];
    float ts_vels[num_bodies][3][1000];
    float body_masses[num_bodies];

    // Format the 2D array into starting statistics for time-series arrays - This essentially formats the positions, velocities, and the masses of the planets.
    for (int n = 0; n < num_bodies; n++) {
        for (int j = 1; j < 4; j++) {
            ts_pos[n][j - 1][0] = data[n][j];

        }
        for (int j = 4; j < 7; j++) {
            ts_vels[n - 1][j - 1][0] = data[n][j];
        }
        body_masses[n] = data[n][8];
        
    }

    // Positional array printing formatter from the CSV file (Debugging)
    for (int i = 0; i < num_rows; i++) {

        for (int j = 1; j < 4; j++) {
            printf("%f ", ts_pos[i][j][0]);
        }
        printf("\n");
    }

    // Velocity array printing formatter from the CSV file (Debugging)
    for (int i = 0; i < num_rows; i++) {

        for (int j = 4; j < 7; j++) {
            printf("%f ", ts_vels[i][j][0]);
        }
        printf("\n");
    }
    // Body mass array printing formatter from the CSV file (Debugging)
    for (int i = 0; i < num_rows; i++) {
        printf("%f\n", body_masses[i]);
    }
    fclose(fp);
    // Define some empty floats that will be used later, also sets the starting time and step.
    float time = 0;
    int step = 0;
    float gravity = 6.67e-11;
    float XPos1, YPos1, ZPos1, XVel1, YVel1, ZVel1, XGravAccel1, YGravAccel1, ZGravAccel1, Accel_Mag, body2_mass, XPos2, YPos2, ZPos2, AngleX, AngleY, AngleZ, XSeparation, Separation, YSeparation, ZSeparation;


    // The c integer is the amount of times that the simulation will loop. Set a higher maximum value and it will run for longer.
    for (int c = 0; c < amount_of_loops; c++) {
        if (c > 0) {
            for (int n = 1; n < num_bodies; n++) {
                for (int p = 0; p < 3; p++) {
                    // Resets the arrays and uses the last run step to unclog the arrays and reduce their size. It starts from the beginning position once more.
                    ts_pos[n][p][1] = ts_pos[n][p][step_amount];
                    ts_vels[n][p][1] = ts_vels[n][p][step_amount];
                }   
            
            step = 1;
            }
        }
        // This while loop runs while the amount of run steps are below the maximum.
        while (step < step_amount) {
            // This for loop runs through every active body to compare against other bodies in a time step.
            for (int n = 1; n < num_bodies; n++) {
                // These variables aren't needed, but are easier to visually see. They represent the current active body's state.
                XPos1 = ts_pos[n][0][step];
                YPos1 = ts_pos[n][1][step];
                ZPos1 = ts_pos[n][2][step];

                XVel1 = ts_vels[n][0][step];
                YVel1 = ts_vels[n][1][step];
                ZVel1 = ts_vels[n][2][step];

                // The vector components of the acceleration are set to 0 here.
                XGravAccel1 = 0;
                YGravAccel1 = 0;
                ZGravAccel1 = 0;
                // This for loop runs through every body that isn't the active body, and calculates their relative gravitational pulls.
                for (int n2 = 1; n2 < num_bodies; n2++) {
                    if (n2 != n) {
                        // Much like the variables for the active body, the variables for the other body are defined by n2.
                        body2_mass = body_masses[n2];

                        XPos2 = ts_pos[n2][0][step];
                        YPos2 = ts_pos[n2][1][step];
                        ZPos2 = ts_pos[n2][2][step];

                        // The seperation between the bodies in all dimensions is calculated.
                        XSeparation = (XPos1 - XPos2);
                        YSeparation = (YPos1 - YPos2);
                        ZSeparation = (ZPos1 - ZPos2);
                        
                        // The magnitude of the resulting vector is calculated.
                        Separation = sqrt(((XSeparation * XSeparation) + (YSeparation * YSeparation) + (ZSeparation * ZSeparation)));
                        // From this the raw acceleration is defined, using the second body's mass, the universal gravitational constant, and the raw separation.
                        Accel_Mag = (gravity * body2_mass) / (Separation * Separation);
                        
                        // The angles between the magnitude of the separation vector and the individual directional vectors are calculated using arc cos.
                        AngleX = acos(XSeparation / Separation);
                        AngleY = acos(YSeparation / Separation);
                        AngleZ = acos(ZSeparation / Separation);

                        // These angles are used with the raw acceleration to give the acceleration in each direction.
                        XGravAccel1 += (-1 * Accel_Mag * cos(AngleX));
                        YGravAccel1 += (-1 * Accel_Mag * cos(AngleY));
                        ZGravAccel1 += (-1 * Accel_Mag * cos(AngleZ));

                    }
                }

                // The acceleration in the directional vectors is combined with the dt, or the relative time passed, and is then added to the current step's velocities.
                ts_vels[n][0][step + 1] = ts_vels[n][0][step] + (XGravAccel1 * step_freq);
                ts_vels[n][1][step + 1] = ts_vels[n][1][step] + (YGravAccel1 * step_freq);  
                ts_vels[n][2][step + 1] = ts_vels[n][2][step] + (ZGravAccel1 * step_freq);

                // The accelerations are set back to 0 so they don't carry over to the next body.
                XGravAccel1 = 0;
                YGravAccel1 = 0;
                ZGravAccel1 = 0;
                // Same thing as the velocities, but using the velocities and current cardinal positions.
                ts_pos[n][0][step + 1] = ts_pos[n][0][step] + ((ts_vels[n][0][step]) * step_freq);
                ts_pos[n][1][step + 1] = ts_pos[n][1][step] + ((ts_vels[n][1][step]) * step_freq);
                ts_pos[n][2][step + 1] = ts_pos[n][2][step] + ((ts_vels[n][2][step]) * step_freq);

            }
            // The current step and time are updated.
            step += 1;
            time += (1 * step_freq);

            }
        // The arrays are appended to the output.csv file, where the data is stored to be graphed in Python.
        FILE *fp2;
        fp2 = fopen("output.csv", "a");
        for (int t = 1; t < step_amount; t++) {
            for (int n = 1; n < num_bodies; n++) {
                for (int p = 0; p < 3; p++) {
                    fprintf(fp2, "%f", ts_pos[n][p][t]);
                    fprintf(fp2, ",");
                }     
            }
            fprintf(fp2, "\n");
        }
        fclose(fp2);
    }
}