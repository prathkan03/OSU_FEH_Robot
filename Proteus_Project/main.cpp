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
#include <math.h>

// Declarations for encoders & motors
FEHMotor right_motor(FEHMotor::Motor1, 9.0);
FEHMotor left_motor(FEHMotor::Motor0, 9.0);
DigitalEncoder right_encoder(FEHIO::P2_1);
DigitalEncoder left_encoder(FEHIO::P2_0);
AnalogInputPin cds(FEHIO::P0_0);
FEHServo robot_arm(FEHServo::Servo1);
FEHServo cart(FEHServo::Servo7);
// Number of points of interest (i.e. A, B, C, D)
#define NUM_POINTS_OF_INTEREST 4

// RPS Delay time
#define RPS_WAIT_TIME_IN_SEC 0.35

// Shaft encoding counts for CrayolaBots
#define COUNTS_PER_INCH 28.92073
#define COUNTS_PER_DEGREE 2.48

/* Defines for how long each pulse should be and at what motor power.
These value will normally be small, but you should play around with the values to find what works best */
#define PULSE_TIME 0.15
#define PULSE_POWER 13

// Define for the motor power while driving (not pulsing)
#define POWER_DRIVE 35

#define HEADING_TOLERANCE 3

/* Direction along axis which robot is traveling
Examples:
    - if robot is traveling to the upper level, that is a PLUS as the y-coordinate is increasing
    - if robot is traveling to the lower level, that is a MINUS as the y-coordinate is decreasing
*/
#define PLUS 0
#define MINUS 1

void test_cds()
{
    while (1)
    {
        LCD.Write(cds.Value());
        Sleep(0.5);
        LCD.Clear();
    }
}
void stop_motor()
{
    right_motor.Stop();
    left_motor.Stop();
}

void move_forward_sleep(int percent, float time)
{
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent + 2);
    Sleep(time);
    stop_motor();
}

void move_forward(int percent, float distance)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent

    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2.0 < (distance * 28.92073))
    {
        // If left motor is stronger, increase right motor percentage
        if (left_encoder.Counts() > right_encoder.Counts() + 10)
        {
            right_motor.SetPercent(percent + 1);
        }
        // If right motor is stronger, increase left motor percentage
        else if (right_encoder.Counts() > left_encoder.Counts() + 10)
        {
            left_motor.SetPercent(percent + 1);
        }
        // Else reset to equal motor percentage
        else
        {
            right_motor.SetPercent(percent);
            left_motor.SetPercent(percent);
        }
    }
    LCD.Clear();
    LCD.WriteLine(left_encoder.Counts());
    LCD.WriteLine(right_encoder.Counts());

    // Turn off motors
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
    while ((left_encoder.Counts() + right_encoder.Counts()) / 2. < (distance * 28.92073))
    {
        LCD.Clear();
    }

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
void robot_turn(int percentR, int percentL, float time)
{
    right_motor.SetPercent(percentR);
    left_motor.SetPercent(percentL);
    Sleep(time);
    stop_motor();
}

// RPS FUNCTIONS

void pulse_counterclockwise(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    Sleep(RPS_WAIT_TIME_IN_SEC);
}
/*
 * Pulse forward a short distance using time
 */
void pulse_forward(int percent, float seconds)
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    // Wait for the correct number of seconds
    Sleep(.1);
}

void check_x(float x_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = PULSE_POWER;
    if (orientation == MINUS)
    {
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.X() > 0 && (RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1))
    {
        if (RPS.X() > x_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(-power, PULSE_TIME);
        }
        else if (RPS.X() < x_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/*
 * Use RPS to move to the desired y_coordinate based on the orientation of the QR code
 */
void check_y(float y_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = PULSE_POWER;
    if (orientation == MINUS)
    {
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while (RPS.Y() > 0 && (RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1))
    {
        if (RPS.Y() > y_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(-power, PULSE_TIME);
        }
        else if (RPS.Y() < y_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/*
 * Use RPS to move to the desired heading
 */
/*
 * Use RPS to move to the desired heading
 */
void check_heading(float heading)
{
    float low = heading - HEADING_TOLERANCE;
    float high = heading + HEADING_TOLERANCE;
    bool check = false;

    if (low < 0)
    {
        low += 360;
        check = true;
    }
    if (high >= 360)
    {
        high -= 360;
        check = true;
    }
    // LCD.WriteLine(low + '\t' + high);

    if (heading > RPS.Heading())
    {
        if (heading - RPS.Heading() > 180)
        {
            while (RPS.Heading() < low || RPS.Heading() > high)
            {
                pulse_counterclockwise(-PULSE_POWER, PULSE_TIME);
                LCD.WriteLine(RPS.Heading());
                if (RPS.Heading() > low || RPS.Heading() < high && RPS.Heading() >= -0.1)
                {
                    break;
                }
            }
        }
        else
        {
            while (RPS.Heading() < low)
            {
                pulse_counterclockwise(PULSE_POWER, PULSE_TIME);
                LCD.WriteLine(RPS.Heading());
                if (RPS.Heading() > low)
                {
                    break;
                }
            }
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }

    else if (heading < RPS.Heading())
    {
        if (RPS.Heading() - heading > 180)
        {
            while (RPS.Heading() < low || RPS.Heading() > high)
            {
                pulse_counterclockwise(PULSE_POWER, PULSE_TIME);
                LCD.WriteLine(RPS.Heading());
                if (RPS.Heading() > low || RPS.Heading() < high && RPS.Heading() >= -0.1)
                {
                    break;
                }
            }
        }
        else
        {
            while (RPS.Heading() > high)
            {
                pulse_counterclockwise(-PULSE_POWER, PULSE_TIME);
                LCD.WriteLine(RPS.Heading());
                if (RPS.Heading() < high)
                {
                    break;
                }
            }
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

void check_heading_zero(float heading){
     float low = heading - HEADING_TOLERANCE;
    float high = heading + HEADING_TOLERANCE;
    if (low < 0)
    {
        low += 360;

    }
    if (high >= 360)
    {
        high -= 360;

    }
     while (RPS.Heading() > high)
            {
                pulse_counterclockwise(-PULSE_POWER, PULSE_TIME);
                LCD.WriteLine(RPS.Heading());
                if (RPS.Heading() < high||RPS.Heading()> low)
                {
                    break;
                }
            }
        
        Sleep(RPS_WAIT_TIME_IN_SEC);
}




void set_points_of_interest()
{
    // Declare variables
    float touch_x, touch_y;
    char points[NUM_POINTS_OF_INTEREST] = {'A', 'B', 'C', 'D'};

    // Open SD file for writing
    FEHFile *fptr = SD.FOpen("RPS_POIs.txt", "w");

    Sleep(100);
    LCD.Clear();

    // Wait for touchscreen to be pressed and released
    LCD.WriteLine("Press Screen to Record");
    while (!LCD.Touch(&touch_x, &touch_y))
        ;
    while (LCD.Touch(&touch_x, &touch_y))
        ;

    LCD.ClearBuffer();

    // Clear screen
    Sleep(100); // wait for 100ms to avoid updating the screen too quickly
    LCD.Clear();

    // Write initial screen info
    LCD.WriteRC("X Position:", 11, 0);
    LCD.WriteRC("Y Position:", 12, 0);
    LCD.WriteRC("   Heading:", 13, 0);

    // Step through each path point to record position and heading
    for (int n = 0; n < NUM_POINTS_OF_INTEREST; n++)
    {
        // Write point letter
        LCD.WriteRC("Touch to set point ", 9, 0);
        LCD.WriteRC(points[n], 9, 20);

        // Wait for touchscreen to be pressed and display RPS data
        while (!LCD.Touch(&touch_x, &touch_y))
        {
            LCD.WriteRC(RPS.X(), 11, 12);       // update the x coordinate
            LCD.WriteRC(RPS.Y(), 12, 12);       // update the y coordinate
            LCD.WriteRC(RPS.Heading(), 13, 12); // update the heading

            Sleep(100); // wait for 100ms to avoid updating the screen too quickly
        }
        while (LCD.Touch(&touch_x, &touch_y))
            ;
        LCD.ClearBuffer();

        // Print RPS data for this path point to file
        SD.FPrintf(fptr, "%f %f\n", RPS.X(), RPS.Y());
    }

    // Close SD file
    SD.FClose(fptr);
    LCD.Clear();
}

void lever_task()
{

    // Turn left towards 90 degree orientation to face lever
    robot_turn(25, -25, 0.48);
    Sleep(1.0);
    check_heading(90);
    Sleep(1.0);

    // move forward AB distance using shaft encoding
    // move_forward(POWER_DRIVE, 4);
    move_forward(POWER_DRIVE, 4);
    Sleep(1.0);
    while (RPS.Heading() < -1) {
        move_forward(-POWER_DRIVE, 0.25);
        Sleep(0.5);
    }
    check_heading(90);
    Sleep(1.0);

    // move arm down
    int deg = 180;
    while (deg > 100)
    {
        deg -= 10;
        robot_arm.SetDegree(deg);
        Sleep(.1);
    }

    robot_arm.SetDegree(180);

    // move Backward BC distance using shaft encoding
    move_forward(-POWER_DRIVE, 2);
    check_heading(90);
    Sleep(1.0);

    // Set arm below lever
    deg = 80;
    robot_arm.SetDegree(deg);
    Sleep(3.0); // Sleep while gassing robot

    // move forward AB distance using shaft encoding
    move_forward(POWER_DRIVE, 2);
    Sleep(0.5);
    while (RPS.Heading() < -1) {
        move_forward(-POWER_DRIVE, 0.25);
        Sleep(0.5);
    }
    // check to see if the orientation is still the same
    check_heading(91);
    // Sleep(1.0);
    // robot_turn(25,-25,.15);
    Sleep(1.0);

    robot_arm.SetDegree(110);
    Sleep(0.3);
    robot_arm.SetDegree(76);
    Sleep(1.0);
    // robot_turn(-25,25,.15);2
    // Sleep(1.0);

    // move back BC distance using shaft encoding
    move_forward(-POWER_DRIVE, 2);
    check_heading(90);

    // Sleep(0.5);

}

// // Sleep a bit

// // move forward 4 inches
// // turn left
// //move forward 34 inches
// //turn left 90 degrees
// //move forward 13.5 inches
// //turns right 90 degrees
// // moves forward 10 inches
// // sense light
// // make movements

// // RED @ START = .182
// // BLACK @ START  = 3.1
// // RED ON COURSE =  3.001
// // RED @ COURSE LIGHT =  .192
// // ROBOT NOT EXACTLY ON COURSE RED LIGHT = 2.9
// // BLUE ON COURSE = .45

int main(void)
{
    
    cart.SetDegree(150);
    RPS.InitializeTouchMenu();
    Sleep(1.0);
    // AWAIT LIGHT VALUE FOR CDS CELL
    while (cds.Value() > 0.65)
    {
        LCD.Write("Waiting for Light :/ ");
        LCD.WriteLine(cds.Value());
        Sleep(1.0);
        LCD.Clear();
    }
    // SENSED LIGHT
    LCD.Write("Light has been sensed :)");
    LCD.WriteLine(cds.Value());
    Buzzer.Beep();

    // // // // LEVER TASK
    lever_task();
    Sleep(1.0);

    // Straighten robot
    robot_turn(-25, 25, 0.7);
    Sleep(1.0);
    check_heading_zero(0);
    Sleep(0.5);
     // reset arm
    robot_arm.SetDegree(180);
    Sleep(1.0);
    robot_turn(-25, 25, .35);
    Sleep(1.0);

    // move up
    move_forward(POWER_DRIVE, 9.5);
    // check_heading(0);
    Sleep(0.5);
    robot_turn(25, -25, .2);
    Sleep(0.5);
    check_heading(350);
    Sleep(.5);

    // //Straighten robot
    // robot_turn(-25,25,0.84);
    // check_heading(270);
    // Sleep(0.5);

    // move forward towards right side
    // move_forward(POWER_DRIVE,3);
    // Sleep(0.5);

    // //Straighten up before ramp
    // robot_turn(25,-25,.6);
    // check_heading(340);
    // Sleep(0.5);

    // Move up ramp straight
    move_forward_sleep(60, 1.63);
    Sleep(0.5);

    // turn to 90 degrees
    robot_turn(25, -25, .85);
    check_heading(101);
    Sleep(0.5);

    //move towards luggage
    move_forward(35, 4);
    Sleep(.5);

    //turn towards luggage
    robot_turn(25, -25, .2);
    Sleep(.5);

    //drop luggage
    cart.SetDegree(50);
    Sleep(.5);

    //turn back to 90 degree
    robot_turn(-25, 25, .2);
    check_heading(90);
    Sleep(.5);

    //go towards ticketing kiosk
    move_forward(25,2);
    check_heading(90);
    Sleep(1.0);
    robot_turn(-25, 25, .7);
    check_heading_zero(0);
    Sleep(.5);
    move_forward_sleep(40, 3.5);
    Sleep(0.5);


    //turn towards passport
    robot_turn(-25, 25, 0.7);
    Sleep(0.5);
    check_heading(270);
    Sleep(1.0);

    //straighten up for passport
    move_forward(-40, 2);
    Sleep(.2);
    Sleep(1.0);

    robot_arm.SetDegree(75);
    Sleep(1.0);

    move_forward(25, 3);
    check_heading(270);
    Sleep(1.0);
    robot_arm.SetDegree(135);
    Sleep(1.0);
    robot_turn(25,-25,.2);
    Sleep(1.0);
    move_forward(25, 1);
    Sleep(1.0);
    robot_turn(-25,25,.2);
    Sleep(1.0);
    move_forward(-25, 4);
    Sleep(.2);
    check_heading(270);
    Sleep(.2);
    robot_arm.SetDegree(76);
    Sleep(1.0);
    robot_turn(-25,25,0.7);
    Sleep(1.0);
    check_heading(200);
    Sleep(1.0);
    move_forward(PULSE_POWER+15,15);
    Sleep(1.0);
    robot_turn(-25,25,.2);
    Sleep(0.5);
    check_heading(180);
    Sleep(1.0);
    move_forward_sleep(35,5);
    Sleep(1.0);
    robot_turn(25,-25,1);

//old code
    // robot_turn(-25, 25, .8);
    // Sleep(1.0);


    // robot_turn(-25,25,.60);
    // check_heading(0);
    // Sleep(1.0);

    // move_forward(POWER_DRIVE,13);
    // LCD.Clear();
    // Sleep(1.0);
    // if(cds.Value()>.2 && cds.Value()<.45){
    //     LCD.SetBackgroundColor(BLUE);
    //     Sleep(1.0);
    //     move_forward(-25, 3);
    //     Sleep(1.0);
    //     robot_turn(-25, 25, .7);
    //     check_heading(270);
    //     Sleep(1.0);
    //     move_forward(25, 3);
    //     Sleep(1.0);
    //     robot_turn(25, -25, .7);
    //     check_heading(0);
    //     move_forward_sleep(25, 3);

    // }
    // else{
    //     LCD.SetBackgroundColor(RED);
    //     Sleep(1.0);
    //     move_forward(-25, 3);
    //     Sleep(1.0);
    //     robot_turn(-25, 25, .7);
    //     check_heading(270);
    //     Sleep(1.0);
    //     move_forward(25, 6);
    //     Sleep(1.0);
    //     robot_turn(25, -25, .7);
    //     check_heading(0);
    //     move_forward_sleep(25, 3);
    // }
    //
    // LCD.Clear();

    //     Sleep(1.0);
    //     robot_turn(-25,25,1.1);
    //     check_heading(0);
    //     Sleep(1.0);

    //     //Move up to kiosk straight
    //     move_forward_sleep(35,2);
    //     check_heading(0);
    //     Sleep(0.5);
    //     move_forward_sleep(35,2);
    //     check_heading(0);
    //     Sleep(0.5);
    //     move_forward_sleep(35,2);
    //     check_heading(0);
    //     Sleep(1.0);

    //     move_forward(-25, 17);
    //     check_heading(0);
    //     Sleep(1.0);

    //     robot_turn(-25, 25, .8);
    //     check_heading(270);
    //     Sleep(1.0);

    //     move_forward_sleep(25, 4);
    //     Sleep(1.0);

    //     robot_turn(-25, 25, .8);
    //     check_heading(180);
    //     Sleep(1.0);

    //     move_forward(35, 15);

    //     Sleep(1.0);

    //     check_heading(180);

    //     move_forward_sleep(35, 6);

    //     // check_heading(0);
    //     Sleep(1.0);
    //     move_forward(-35,3);
    //     Sleep(1.0);
    //     robot_turn(-25,25,.8);
    //     robot_arm.SetDegree(80);

    //     Sleep(1.0);
    //     move_forward(35,2);
    //     Sleep(1.0);

    // //move arm up
    // int deg = 80;
    // while(deg<140){
    //     deg+=10;
    //     robot_arm.SetDegree(deg);
    //     Sleep(.1);
    // }

    //     Sleep(1.0);
    //     move_forward(-35,4);
    //     Sleep(1.0);

    //         //turn to 90 degrees
    //     robot_turn(25,-25,.82);
    //     check_heading(90);
    //     Sleep(1.0);

    //     move_forward(35,12);

    //     Sleep(1.0);

    //     //turn to 90 degrees
    //     robot_turn(-25,25,.8);

    //     Sleep(1.0);

    //     move_forward(35,28);

    //     Sleep(1.0);

    //     move_forward(-35,24);
}

// // // // //Float values to store desired heading, x and y values, and distance
// // // // float EFHeading = RPS.Heading();
// // // // float Fx = 29;
// // // // float Fy = 14.4;
// // // // float EF = 8;

// // // // move_forward(-POWER, EF);
// // // // //check to see if the orientation is still the same
// // // // check_heading(90);
// // // // //check to see if x position matches desired x
// // // // check_x(Fx,0);
// // // // //check to see if the orientation is still the same
// // // // check_heading(0);

// // // // //Float values to store desired heading, x and y values, and distance
// // // // float FGHeading = 358.6;
// // // // float Gx = 29.5;
// // // // float Gy = 43;
// // // // float FG = 30;

// // // // move_forward(40, FG);
// // // // //check to see if the orientation is still the same
// // // // check_heading(0);
// // // // //check to see if x position matches desired x
// // // // check_y(Fy,0);
// // // // //check to see if the orientation is still the same
// // // // check_heading(0);

// // // // check_heading(90);

// // // // move_forward(25,7);
// // // // check_heading(90);
// // // // check_x(22.1,1);
// // // // check_heading(90);

// // // // move_forward(25,10);
// // // // check_heading(90);
// // // // check_x(11.3,1);
// // // // check_heading(90);

// // // // Sleep(2.0);
// // // // RPS.InitializeTouchMenu();
// // // // set_points_of_interest();

// // // // float b_orientation = 0;
// // // // //rotates to correct angle
// // // // check_heading(b_orientation);

// // // // float BC = 7.0;
// // // // float BC_Heading = RPS.Heading();
// // // // float C_x = 19.099;
// // // // float C_y = 26.1;

// // // // //moves forward to desired location using shaft encoding
// // // // move_forward(POWER, BC);
// // // // //rotate to desired angle
// // // // check_heading(BC_Heading);
// // // // //pulses toward desired x location if off
// // // // check_x(C_x, 0);
// // // // //pulses toward desired y location if off
// // // // check_y(C_y, 0);
// // // // //moves toward desired heading if off
// // // // check_heading(BC_Heading);

// // // // // //robot arm

// // // //lever task
// // // float CD = BC - 2;
// // // float CD_Heading = RPS.Heading();
// // // float D_x = 19.099;
// // // float D_y = 20.1;

// // // move_forward(-POWER, CD);
// // // //rotate to desired angle
// // // check_heading(CD_Heading);
// // // //pulses toward desired x location if off
// // // check_x(CD_x, 0);
// // // //pulses toward desired y location if off
// // // check_y(CD_y, 1);

// // // //Turn
// // // float d_orientation = 90;
// // // check_heading(d_orientation);

// // // float DE = 10;
// // // float DE_x;
// // // float DE_y;
// // // float DE_Heading = RPS.Heading();

// // // move_forward(POWER, DE);
// // // //rotate to desired angle
// // // check_heading(DE_Heading);
// // // //pulses toward desired x location if off
// // // check_x(DE_x, 1);
// // // //pulses toward desired y location if off
// // // check_y(DE_y, 0);
// // // //moves toward desired headihng if off
// // // check_heading(DE_Heading);

// // // float e_orientation;
// // // //lever

// // //     // Get correct lever from the RPS
// // //     int correctLever = RPS.GetCorrectLever();

// // //     // Check which lever to flip and perform some action
// // //     if(correctLever == 0)
// // //     {
// // //         // Perform actions to flip left lever
// // //     }
// // //     else if(correctLever == 1)
// // //     {
// // //         // Perform actions to flip middle lever
// // //     }
// // //     else if(correctLever == 2)
// // //     {
// // //        // Perform actions to flip right lever
// // //     }

// // }
// }