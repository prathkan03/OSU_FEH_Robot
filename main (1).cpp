#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHAccel.h>
#include <FEHBattery.h>
#include <FEHBuzzer.h>
#include <FEHRPS.h>
#include <FEHSD.h>
#include <string.h>
#include <stdio.h>

//Declarations for encoders & motors
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
DigitalEncoder right_encoder(FEHIO::P2_0);
DigitalEncoder left_encoder(FEHIO::P2_1);
AnalogInputPin cds (FEHIO::P0_0);
FEHServo robot_arm(FEHServo::Servo1);
// Number of points of interest (i.e. A, B, C, D)
#define NUM_POINTS_OF_INTEREST 4

// RPS Delay time
#define RPS_WAIT_TIME_IN_SEC 0.35

// Shaft encoding counts for CrayolaBots
#define COUNTS_PER_INCH 40.5
#define COUNTS_PER_DEGREE 2.48

// Define for the motor power while driving (not pulsing)
#define POWER 20

#define HEADING_TOLERANCE 2

/* Direction along axis which robot is traveling
Examples:
	- if robot is traveling to the upper level, that is a PLUS as the y-coordinate is increasing
	- if robot is traveling to the lower level, that is a MINUS as the y-coordinate is decreasing
*/
#define PLUS 0
#define MINUS 1

// void set_points_of_interest()
// {
//     // Declare variables
//     float touch_x, touch_y;
//     char points[NUM_POINTS_OF_INTEREST] = {'A', 'B', 'C', 'D'};
//     float touch_x, touch_y;

//     // Open SD file for writing
//     FEHFile *fptr = SD.FOpen("RPS_POIs.txt", "w");

//     Sleep(100);
//     LCD.Clear();

//     // Wait for touchscreen to be pressed and released
//     LCD.WriteLine("Press Screen to Record");
//     while (!LCD.Touch(&touch_x, &touch_y));
//     while (LCD.Touch(&touch_x, &touch_y));

//     LCD.ClearBuffer();

//     // Clear screen
//     Sleep(100); // wait for 100ms to avoid updating the screen too quickly
//     LCD.Clear();

//     // Write initial screen info
//     LCD.WriteRC("X Position:", 11, 0);
//     LCD.WriteRC("Y Position:", 12, 0);
//     LCD.WriteRC("   Heading:", 13, 0);

//     // Step through each path point to record position and heading
//     for (int n = 0; n < NUM_POINTS_OF_INTEREST; n++)
//     {
//         // Write point letter
//         LCD.WriteRC("Touch to set point ", 9, 0);
//         LCD.WriteRC(points[n], 9, 20);

//         // Wait for touchscreen to be pressed and display RPS data
//         while (!LCD.Touch(&touch_x, &touch_y))
//         {
//             LCD.WriteRC(RPS.X(), 11, 12);       // update the x coordinate
//             LCD.WriteRC(RPS.Y(), 12, 12);       // update the y coordinate
//             LCD.WriteRC(RPS.Heading(), 13, 12); // update the heading

//             Sleep(100); // wait for 100ms to avoid updating the screen too quickly
//         }
//         while (LCD.Touch(&touch_x, &touch_y));
//         LCD.ClearBuffer();

//         // Print RPS data for this path point to file
//         SD.FPrintf(fptr, "%f %f\n", RPS.X(), RPS.Y());
//     }

//     // Close SD file
//     SD.FClose(fptr);
//     LCD.Clear();
// }

void test_cds(){
    while(1){
    LCD.Write(cds.Value());
    Sleep(0.5);
    LCD.Clear();
}
}

void move_forward_sleep(int percent, float time){
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);
    Sleep(time);
}

void move_forward(int percent, float distance){
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent-5);
    left_motor.SetPercent(percent);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < (distance * 28.92073));

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}


void stop_motor(){
    right_motor.Stop();
    left_motor.Stop();
}
void turn(int percentR, int percentL, int distance)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percentR);
    left_motor.SetPercent(percentL);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < (distance * 28.92073));

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
void robot_turn(int percentR, int percentL, float time){
    right_motor.SetPercent(percentR);
    left_motor.SetPercent(percentL);
    Sleep(time);
    stop_motor();

}

// move forward 4 inches
// turn left
//move forward 34 inches
//turn left 90 degrees
//move forward 13.5 inches
//turns right 90 degrees
// moves forward 10 inches
// sense light
// make movements

// RED @ START = .182
// BLACK @ START  = 3.1
// RED ON COURSE =  3.001
// RED @ COURSE LIGHT =  .192
// ROBOT NOT EXACTLY ON COURSE RED LIGHT = 2.9
// BLUE ON COURSE = .45

int main(void)
{
    //move_forward(60,20);
    // float touch_x, touch_y;

    // float A_x, A_y, B_x, B_y, C_x, C_y, D_x, D_y;

    // RPS.InitializeTouchMenu();

    // set_points_of_interest();

    // LCD.Clear();
    // LCD.WriteLine("Press Screen To Start Run");
    // while (!LCD.Touch(&touch_x, &touch_y));
    // while (LCD.Touch(&touch_x, &touch_y));

    // // COMPLETE CODE HERE TO READ SD CARD FOR LOGGED X AND Y DATA POINTS
    // FEHFile *fptr = SD.FOpen("RPS_POIs.txt", "r");
    // SD.FScanf(fptr, "%f%f", &A_x, &A_y);
    // SD.FScanf(fptr, "%f%f", &B_x, &B_y);
    // SD.FScanf(fptr, "%f%f", &C_x, &C_y);
    // SD.FScanf(fptr, "%f%f", &D_x, &D_y);
    // SD.FClose(fptr);



    // // TEST CDS VALUES
//   int deg = 180;
//      while(deg>0){
//         robot_arm.SetDegree(deg);
//         Sleep(0.5);
//         deg=deg-20;
//      }

    // //AWAIT LIGHT VALUE FOR CDS CELL
    // while(cds.Value() > 0.65){
    //     LCD.Write("Waiting for Light :/ ");
    //     Sleep(.5);
    //     LCD.Clear();
    // }

    // // SENSE CDS VALUE AND MOVE UP THE RAMP TO THE TICKET LIGHT
    // LCD.Clear();
    Sleep(10.0);
    LCD.Write("Light has been sensed :)");
    Buzzer.Beep();
    move_forward(35, 11.5);
    stop_motor();
    LCD.Clear();
    robot_turn(25,-25,.3);
    move_forward(35,2);
    // robot_turn(-25,-25,.2);
    stop_motor();
    int deg = 135;
    while(deg>80){
        robot_arm.SetDegree(deg);
        deg=deg-20;
        Sleep(.2);
    }
    while(deg<135){
        robot_arm.SetDegree(deg);
        deg=deg+20;
        Sleep(.2);
    }
    move_forward_sleep(-35,20);
   
  
    // robot_turn(-30, 30, .35);
    // stop_motor();
    // move_forward(35,34);
    // LCD.Clear();
    // LCD.Write("Power: 45");
    // stop_motor();
    // Sleep(2.0);
    // robot_turn(30, -30, 1);
    // stop_motor();
    // Sleep(1.0);
    // move_forward_sleep(-30, 3);
    // LCD.Clear();
    // LCD.Write("Power: 40");
    // Sleep(1.0);
    // stop_motor();
    // Sleep(1.0);
    // move_forward(25,16);
    // LCD.Clear();
    // LCD.Write("Power: 35");
    // stop_motor();
    // Sleep(1.0);
    // robot_turn(30,-30,1.2);
    // stop_motor();
    // Sleep(1.0);
    // move_forward(30,1);
    // LCD.Clear();
    // LCD.Write("Power: 40");
    // stop_motor();
    // Sleep(5);



    // //phase 1 ends


    // //drop luggage
    // robot_arm.SetDegree(120);
    // Sleep(2.0);
    // int deg = 120;
    // while(deg<75){
    //     deg-=10;
    //     robot_arm.SetDegree(deg);
    //     sleep(.2);
    // }


    // //start phase 2

    // move_forward(-35, 1);
    // Sleep(1.0);
    // stop_motor();
    // robot_turn(40,-40,1.2);
    // Sleep(1.0);
    // stop_motor();
    // move_forward(35, 13); 
    // Sleep(1.0);
    // stop_motor();
    // robot_turn(-40,40,1.2);
    // Sleep(1.0);
    // stop_motor();
    // move_forward(45, 32);
    // stop_motor();
    // robot_turn(-40, 40, .5);
    // stop_motor();
    // move_forward(45, 2);
    // stop_motor();
    // robot_turn(40,-40, 1.2);
    // stop_motor();
    // move_forward_sleep(40, 2);
    // stop_motor();




    // robot_arm.SetDegree(80);
    // Sleep(2.0);
    // move_forward(20,4);
    // stop_motor();
    // Sleep(1.0);
    // int deg = 80;
    // while(deg<120){
    //     deg+=10;
    //     robot_arm.SetDegree(deg);
    //     Sleep(.2);
    // }
    // Sleep(2.0);
    // move_forward(15, 2.5);
    // stop_motor();
    // robot_arm.SetDegree(140);
    // move_forward(15,3.5);
    // stop_motor();
    // Sleep(1.0);
    // move_forward(-15,6);
    // stop_motor();
    
    // int deg = 180;
    // while(deg>0){
    //     deg+=10;
    //     robot_arm.SetDegree(deg);
    //     Sleep(.2);
    // }

}

    // robot_turn(15, -15, 2);
    // move_forward(25, 25);
    // robot_turn(15, -15, 3.5);
    // move_forward_sleep(-25, 5);
    // move_forward(25, )

//     //LEVER PUSH
//     robot_turn(20, -20, .6);
//     move_forward(25,4);
//     int deg =180;
//     while(deg>90){
//         robot_arm.SetDegree(deg);
//         Sleep(0.4);
//         deg=deg-20;
//     }
//     //  while(deg<180){
//     //     robot_arm.SetDegree(deg);
//     //     Sleep(0.4);
//     //     deg=deg+20;
//     // }
//     move_forward(-25,2);
//     LCD.Clear();
//     LCD.WriteLine("Timer started");
//     Sleep(5.0);
//     LCD.Clear();
//     LCD.WriteLine("Timer ended");
//     deg = 80;
//     robot_arm.SetDegree(deg);
//     move_forward(25,2);
//      while(deg<130){
//         robot_arm.SetDegree(deg);
//         Sleep(0.4);
//         deg=deg+20;
//     }
//     move_forward(25,-3);

//     // servo 900 min 1800 max
//      //enter the minimum and maximum servo values
//     //from the .TouchCalibrate() function
//     // robot_arm.SetMin(900);
//     // robot_arm.SetMax(1800);
//     // robot_arm.SetDegree(0);
//     // Sleep(2.0);
//     // robot_arm.SetDegree(180);
// }



// // // void motorTest() {
// // //     right_motor.SetPercent(40);
// // //   Sleep(2.5);
// // //     left_motor.SetPercent(40);
// // //   Sleep(2.5);
// // //     right_motor.SetPercent(-40);
// // //   Sleep(2.5);
// // //     left_motor.SetPercent(-40);
// // //   Sleep(2.5);
// // // }




// // #include <FEHLCD.h>
// // #include <FEHIO.h>
// // #include <FEHUtility.h>
// // #include <FEHMotor.h>
// // #include <FEHRPS.h>
// // #include <FEHSD.h>
// // #include <string.h>
// // #include <stdio.h>
// // #include <math.h>

// // // Number of points of interest (i.e. A, B, C, D)
// // #define NUM_POINTS_OF_INTEREST 4

// // // RPS Delay time
// // #define RPS_WAIT_TIME_IN_SEC 0.35

// // // Shaft encoding counts for CrayolaBots
// // #define COUNTS_PER_INCH 40.5
// // #define COUNTS_PER_DEGREE 2.48

// // /* Defines for how long each pulse should be and at what motor power. 
// // These value will normally be small, but you should play around with the values to find what works best */
// // #define PULSE_TIME 0.1
// // #define PULSE_POWER 15

// // // Define for the motor power while driving (not pulsing)
// // #define POWER 20

// // #define HEADING_TOLERANCE 2

// // /* Direction along axis which robot is traveling
// // Examples:
// // 	- if robot is traveling to the upper level, that is a PLUS as the y-coordinate is increasing
// // 	- if robot is traveling to the lower level, that is a MINUS as the y-coordinate is decreasing
// // */
// // #define PLUS 0
// // #define MINUS 1

// // // Declarations for encoders & motors
// // DigitalEncoder right_encoder(FEHIO::P2_0);
// // DigitalEncoder left_encoder(FEHIO::P2_1);
// // FEHMotor right_motor(FEHMotor::Motor1, 9.0);
// // FEHMotor left_motor(FEHMotor::Motor0, 9.0);

// // void set_points_of_interest()
// // {
// //     // Declare variables
// //     float touch_x, touch_y;
// //     char points[NUM_POINTS_OF_INTEREST] = {'A', 'B', 'C', 'D'};

// //     // Open SD file for writing
// //     FEHFile *fptr = SD.FOpen("RPS_POIs.txt", "w");

// //     Sleep(100);
// //     LCD.Clear();

// //     // Wait for touchscreen to be pressed and released
// //     LCD.WriteLine("Press Screen to Record");
// //     while (!LCD.Touch(&touch_x, &touch_y));
// //     while (LCD.Touch(&touch_x, &touch_y));

// //     LCD.ClearBuffer();

// //     // Clear screen
// //     Sleep(100); // wait for 100ms to avoid updating the screen too quickly
// //     LCD.Clear();

// //     // Write initial screen info
// //     LCD.WriteRC("X Position:", 11, 0);
// //     LCD.WriteRC("Y Position:", 12, 0);
// //     LCD.WriteRC("   Heading:", 13, 0);

// //     // Step through each path point to record position and heading
// //     for (int n = 0; n < NUM_POINTS_OF_INTEREST; n++)
// //     {
// //         // Write point letter
// //         LCD.WriteRC("Touch to set point ", 9, 0);
// //         LCD.WriteRC(points[n], 9, 20);

// //         // Wait for touchscreen to be pressed and display RPS data
// //         while (!LCD.Touch(&touch_x, &touch_y))
// //         {
// //             LCD.WriteRC(RPS.X(), 11, 12);       // update the x coordinate
// //             LCD.WriteRC(RPS.Y(), 12, 12);       // update the y coordinate
// //             LCD.WriteRC(RPS.Heading(), 13, 12); // update the heading

// //             Sleep(100); // wait for 100ms to avoid updating the screen too quickly
// //         }
// //         while (LCD.Touch(&touch_x, &touch_y));
// //         LCD.ClearBuffer();

//         // Print RPS data for this path point to file
//         SD.FPrintf(fptr, "%f %f\n", RPS.X(), RPS.Y());
//     }

//     // Close SD file
//     SD.FClose(fptr);
//     LCD.Clear();
// }

// /*
//  * Pulse forward a short distance using time
//  */
// void pulse_forward(int percent, float seconds)
// {
//     // Set both motors to desired percent
//     right_motor.SetPercent(percent);
//     left_motor.SetPercent(percent);

//     // Wait for the correct number of seconds
//     Sleep(seconds);

//     // Turn off motors
//     right_motor.Stop();
//     left_motor.Stop();
// }

// /*
//  * Pulse counterclockwise a short distance using time
//  */
// void pulse_counterclockwise(int percent, float seconds)
// {
//     // Set both motors to desired percent
//     right_motor.SetPercent(percent);
//     left_motor.SetPercent(-percent);

//     // Wait for the correct number of seconds
//     Sleep(seconds);

//     // Turn off motors
//     right_motor.Stop();
//     left_motor.Stop();
// }

// /*
//  * Move forward using shaft encoders where percent is the motor percent and counts is the distance to travel
//  */
// void move_forward(int percent, int counts) // using encoders
// {
//     // Reset encoder counts
//     right_encoder.ResetCounts();
//     left_encoder.ResetCounts();

//     // Set both motors to desired percent
//     right_motor.SetPercent(percent);
//     left_motor.SetPercent(percent);

//     // While the average of the left and right encoder are less than counts,
//     // keep running motors
//     while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

//     // Turn off motors
//     right_motor.Stop();
//     left_motor.Stop();
// }

// /*
//  * Turn counterclockwise using shaft encoders where percent is the motor percent and counts is the distance to travel
//  */
// void turn_counterclockwise(int percent, int counts)
// {
//     // Reset encoder counts
//     right_encoder.ResetCounts();
//     left_encoder.ResetCounts();

//     // Set both motors to desired percent
//     right_motor.SetPercent(percent);
//     left_motor.SetPercent(-percent);

//     // While the average of the left and right encoder are less than counts,
//     // keep running motors
//     while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

//     // Turn off motors
//     right_motor.Stop();
//     left_motor.Stop();
// }

// /*
//  * Use RPS to move to the desired x_coordinate based on the orientation of the QR code
//  */
// void check_x(float x_coordinate, int orientation)
// {
//     // Determine the direction of the motors based on the orientation of the QR code
//     int power = PULSE_POWER;
//     if (orientation == MINUS)
//     {
//         power = -PULSE_POWER;
//     }

//     // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
//     while (RPS.X()>0 && (RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1))
//     {
//         if (RPS.X()>x_coordinate)
//         {
//             // Pulse the motors for a short duration in the correct direction
//             pulse_forward(-power, PULSE_TIME);
//         }
//         else if (RPS.X()<x_coordinate)
//         {
//             // Pulse the motors for a short duration in the correct direction
//             pulse_forward(power, PULSE_TIME);
//         }
//         Sleep(RPS_WAIT_TIME_IN_SEC);
//     }
// }

// /*
//  * Use RPS to move to the desired y_coordinate based on the orientation of the QR code
//  */
// void check_y(float y_coordinate, int orientation)
// {
//     // Determine the direction of the motors based on the orientation of the QR code
//     int power = PULSE_POWER;
//     if (orientation == MINUS)
//     {
//         power = -PULSE_POWER;
//     }

//     // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
//     while (RPS.Y()>0 && (RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1))
//     {
//         if (RPS.Y()>y_coordinate)
//         {
//             // Pulse the motors for a short duration in the correct direction
//             pulse_forward(-power, PULSE_TIME);
//         }
//         else if (RPS.Y()<y_coordinate)
//         {
//             // Pulse the motors for a short duration in the correct direction
//             pulse_forward(power, PULSE_TIME);
//         }
//         Sleep(RPS_WAIT_TIME_IN_SEC);
//     }
// }


// /*
//  * Use RPS to move to the desired heading
//  */
// void check_heading(float heading)
// {
//         float low = heading -10;
//         float high = heading +10;
//         bool check = false;
    
//         if(low<0){
//             low+=360;
//             check = true;
//         }
//         if(high>=360){
//             high-=360;
//             check = true;
//         }

//         if(check){
//         //   if (RPS.Heading() > heading) {
//         //     while (RPS.Heading()<high||RPS.Heading()>low) {
//         //         pulse_counterclockwise(POWER, PULSE_TIME);
//         //         if(RPS.Heading()<)
//         //     }
//         //     Sleep(RPS_WAIT_TIME_IN_SEC);
//         // }
        
//             while (RPS.Heading()<high||RPS.Heading()<
//             low) {
//                 pulse_counterclockwise(POWER, PULSE_TIME);
//                 if(RPS.Heading()<low&&RPS.Heading()<high){
//                     break;
//                 }
//              }
//             Sleep(RPS_WAIT_TIME_IN_SEC);
//         }
//         else{
//             if (RPS.Heading() > heading) {
//                 int n = 1;
//                 if(abs(RPS.Heading()-heading)>180){
//                     n = -1;
//                 }
//             while (RPS.Heading()>high||RPS.Heading()<low) {
                
//                 pulse_counterclockwise(n*POWER, PULSE_TIME);
//             }
//             Sleep(RPS_WAIT_TIME_IN_SEC);
//         }
//         else {
//             int n = -1;
//                 if(abs(RPS.Heading()-heading)>180){
//                     n = 1;
//                 }
//             while (RPS.Heading()>high||RPS.Heading()<low) {
//                 pulse_counterclockwise(n*POWER, PULSE_TIME);
//             }
//             Sleep(RPS_WAIT_TIME_IN_SEC);
//         } 
//         }

// }



//     //     if (RPS.Heading() > heading) {
//     //         while (RPS.Heading()>high||RPS.Heading()<low) {
//     //             pulse_counterclockwise(POWER, PULSE_TIME);
//     //         }
//     //     }
//     //     else {
//     //         while (RPS.Heading() != heading) {
//     //             pulse_counterclockwise(-POWER, PULSE_TIME);
//     //         }
//     //     }
//     // }
//     // else {
//     //     while(RPS.Heading()<heading-10 || RPS.Heading()>heading+10){
//     //         if(RPS.Heading()>heading){
//     //             pulse_counterclockwise(-POWER, PULSE_TIME);
//     //         }
//     //         else if(RPS.Heading()<heading){
//     //             pulse_counterclockwise(POWER, PULSE_TIME);
//     //         }
//     //         Sleep(RPS_WAIT_TIME_IN_SEC);
//     //     }
//     // }

//     // You will need to fill out this one yourself and take into account
//     // checking for proper RPS data and the edge conditions
//     //(when you want the robot to go to 0 degrees or close to 0 degrees)

//     /*
//         SUGGESTED ALGORITHM:
//         1. Check the current orientation of the QR code and the desired orientation of the QR code
//         2. Check if the robot is within the desired threshold for the heading based on the orientation
//         3. Pulse in the correct direction based on the orientation

//         START: 
//         180 deg
//         Turn 
//     */

// // int main(void)
// // {
    
// //     float touch_x, touch_y;

// //     float A_x, A_y, B_x, B_y, C_x, C_y, D_x, D_y;
// //     float A_heading, B_heading, C_heading, D_heading;
// //     int B_C_counts, C_D_counts, turn_90_counts, turn_180_counts;

// //     RPS.InitializeTouchMenu();

// //     set_points_of_interest();

// //     LCD.Clear();
// //     LCD.WriteLine("Press Screen To Start Run");
// //     while (!LCD.Touch(&touch_x, &touch_y));
// //     while (LCD.Touch(&touch_x, &touch_y));

// //     // COMPLETE CODE HERE TO READ SD CARD FOR LOGGED X AND Y DATA POINTS
// //     FEHFile *fptr = SD.FOpen("RPS_POIs.txt", "r");
// //     SD.FScanf(fptr, "%f%f", &A_x, &A_y);
// //     SD.FScanf(fptr, "%f%f", &B_x, &B_y);
// //     SD.FScanf(fptr, "%f%f", &C_x, &C_y);
// //     SD.FScanf(fptr, "%f%f", &D_x, &D_y);
// //     SD.FClose(fptr);

// //     // WRITE CODE HERE TO SET THE HEADING DEGREES AND COUNTS VALUES
// //     A_heading = 180;
// //     B_heading = 270;
// //     C_heading = 90;
// //     D_heading = 0;

// //     B_C_counts = COUNTS_PER_INCH * 10;
// //     C_D_counts = COUNTS_PER_INCH  * 10;

// //     turn_90_counts = COUNTS_PER_DEGREE * 90;
// //     turn_180_counts = COUNTS_PER_DEGREE * 180;

    
// //     // Open file pointer for writing
// //     fptr = SD.FOpen("RESULTS.txt", "w");

// //     // A --> B
// //     check_y(B_y, PLUS);
// //     check_heading(B_heading);
// //     Sleep(1.0);
    
// //     // COMPLETE CODE HERE TO WRITE EXPECTED AND ACTUAL POSITION INFORMATION TO SD CARD
// //     SD.FPrintf(fptr, "Expected B Position: %f %f %f\n", B_x, B_y, B_heading);
// //     SD.FPrintf(fptr, "Actual B Position:   %f %f %f\n\n", RPS.X(), RPS.Y(), RPS.Heading());

// //     //Log

// //     // B --> C
// //     move_forward(POWER, B_C_counts);
// //     check_x(C_x, MINUS);
// //     turn_counterclockwise(POWER,  turn_180_counts);
// //     check_heading(C_heading);
// //     Sleep(1.0);

// //     // COMPLETE CODE HERE TO WRITE EXPECTED AND ACTUAL POSITION INFORMATION TO SD CARD
// //     SD.FPrintf(fptr, "Expected C Position: %f %f %f\n", C_x, C_y, C_heading);
// //     SD.FPrintf(fptr, "Actual C Position:   %f %f %f\n\n", RPS.X(), RPS.Y(), RPS.Heading());

// //     // C --> D
// //     move_forward(POWER, C_D_counts);
// //     check_x(D_x, PLUS);
// //     turn_counterclockwise(-POWER, turn_90_counts);
// //     check_heading(D_heading);
// //     check_y(D_y, MINUS);
// //     Sleep(1.0);

// //     // COMPLETE CODE HERE TO WRITE EXPECTED AND ACTUAL POSITION INFORMATION TO SD CARD
// //     SD.FPrintf(fptr, "Expected C Position: %f %f %f\n", D_x, D_y, D_heading);
// //     SD.FPrintf(fptr, "Actual C Position:   %f %f %f\n\n", RPS.X(), RPS.Y(), RPS.Heading());
    
// //     // Close file pointer
// //     SD.FClose(fptr);

// //     return 0;
// // }
