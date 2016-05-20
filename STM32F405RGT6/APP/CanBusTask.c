#include "main.h"

float target_pitch_angle;
float target_yaw_angle;

// yaw and pitch angle rx messages from CAN
int16_t measured_yaw_angle;   // range from 0~8191, 0x1FFF
int16_t measured_pitch_angle; // range from 0~8191, 0x1FFF

// Beck read from the datasheet, and guess it is the measured current
// yaw and pitch measured current rx messages from CAN
int16_t measured_yaw_current;
int16_t measured_pitch_current;

// yaw and pitch current rx messages from CAN
int16_t target_yaw_current;
int16_t target_pitch_current;

int16_t measured_201_angle;
int16_t measured_201_speed;
int16_t x145;
int16_t x167;

int16_t measured_202_angle;
int16_t measured_202_speed;
int16_t x245;
int16_t x267;

int16_t measured_203_angle;
int16_t measured_203_speed;
int16_t x345;
int16_t x367;

int16_t measured_204_angle;
int16_t measured_204_speed;
int16_t x445;
int16_t x467;

/*******
RX addresses
0x201: Front right wheel
0x202: Front left wheel
0x203: Back left wheel
0x204: Back right wheel
0x205: Yaw
0x206: Pitch

All rx messages mapped the same way:
data 0 and 1 measure angle
data 4 and 5 relates to what current you are tx to motors
data 4 and 5 NOT same as current tx value
*******/
void CanReceiveMsgProcess(CanRxMsg * rx_message)
{
/***************
Wheel RX Address
0x201: Front right wheel
0x202: Front left wheel
0x203: Back left wheel
0x204: Back right wheel

For sample robot 2015, with EC60 motor drives:
    data 0 and 1 measure position of wheel, from 0 to 8191
    clockwise wheel rotation decreases wheel's position
    wheel position value repeats (decrease from 0 means go back to 8200 again)

    data 4 and 5 relates to what current you are tx to motors
    data 4 and 5 NOT same as current tx value

    data 2, 3, 6, 7 not useful

For robot we build in 2016, with 820R motor drives:
    You calibrate the CAN address in the first time
    data 0 and 1 measure position of wheel, from 0 to 8191
    data 2 and 3 measure rotational speed, in unit of RPM
    data 4,5, 6, and 7 are null.
***************/

   if(rx_message->StdId == 0x201)
    {
        int16_t x1data0 = rx_message->Data[0];
        int16_t x1data1 = rx_message->Data[1];
        int16_t x1data2 = rx_message->Data[2];
        int16_t x1data3 = rx_message->Data[3];
        int16_t x1data4 = rx_message->Data[4];
        int16_t x1data5 = rx_message->Data[5];
        int16_t x1data6 = rx_message->Data[6];
        int16_t x1data7 = rx_message->Data[7];

        measured_201_angle = ( x1data0 << 8 ) | x1data1;
        measured_201_speed = ( x1data2 << 8 ) | x1data3;
        x145 = ( x1data4 << 8 ) | x1data5;
        x167 = ( x1data6 << 8 ) | x1data7;
    }

    if(rx_message->StdId == 0x202)
    {

        int16_t x2data0 = rx_message->Data[0];
        int16_t x2data1 = rx_message->Data[1];
        int16_t x2data2 = rx_message->Data[2];
        int16_t x2data3 = rx_message->Data[3];
        int16_t x2data4 = rx_message->Data[4];
        int16_t x2data5 = rx_message->Data[5];
        int16_t x2data6 = rx_message->Data[6];
        int16_t x2data7 = rx_message->Data[7];
        measured_202_angle = ( x2data0 << 8 ) | x2data1;
        measured_202_speed = ( x2data2 << 8 ) | x2data3;
        x245 = ( x2data4 << 8 ) | x2data5;
        x267 = ( x2data6 << 8 ) | x2data7;

    }

    if(rx_message->StdId == 0x203)
    {
        int16_t x3data0 = rx_message->Data[0];
        int16_t x3data1 = rx_message->Data[1];
        int16_t x3data2 = rx_message->Data[2];
        int16_t x3data3 = rx_message->Data[3];
        int16_t x3data4 = rx_message->Data[4];
        int16_t x3data5 = rx_message->Data[5];
        int16_t x3data6 = rx_message->Data[6];
        int16_t x3data7 = rx_message->Data[7];
        measured_203_angle = ( x3data0 << 8 ) | x3data1;
        measured_203_speed = ( x3data2 << 8 ) | x3data3;
        x345 = ( x3data4 << 8 ) | x3data5;
        x367 = ( x3data6 << 8 ) | x3data7;
    }

    if(rx_message->StdId == 0x204)
    {

        int16_t x4data0 = rx_message->Data[0];
        int16_t x4data1 = rx_message->Data[1];
        int16_t x4data2 = rx_message->Data[2];
        int16_t x4data3 = rx_message->Data[3];
        int16_t x4data4 = rx_message->Data[4];
        int16_t x4data5 = rx_message->Data[5];
        int16_t x4data6 = rx_message->Data[6];
        int16_t x4data7 = rx_message->Data[7];
        measured_204_angle = ( x4data0 << 8 ) | x4data1;
        measured_204_speed = ( x4data2 << 8 ) | x4data3;
        x445 = ( x4data4 << 8 ) | x4data5;
        x467 = ( x4data6 << 8 ) | x4data7;
    }

/************** End of Wheel Motor RX Code and Address **************/
    /************ YAW ************/
    // Yaw angle range is: [around 40, around 4800]
    // 40 is right-most yaw position
    // around 4800 is left-most yaw position
    // data 0 and 1 measure angle

    // data 4 and 5 relates to what current you are tx to motors
    // data 4 and 5 NOT same as current tx value
    // -1000 current value = 27852 (yaw_data4)<<8|(yaw_data5) value
    // -750 current value = 24305 (yaw_data4)<<8|(yaw_data5) value
    // -500 current value = 16630 (yaw_data4)<<8|(yaw_data5) value
    // pitch has same current to (data4<<8)|(data5) conversion
    if(rx_message->StdId == 0x205)
    {

        // construct message from data[0] and data[1]
        int16_t yaw_data0 = rx_message->Data[0];
        int16_t yaw_data1 = rx_message->Data[1];
        int16_t yaw_data4 = rx_message->Data[4];
        int16_t yaw_data5 = rx_message->Data[5];

        measured_yaw_angle = (yaw_data0)<<8|(yaw_data1);
        target_yaw_current = (yaw_data4)<<8|(yaw_data5);

        // normalize angle range since default angle range is werid
        if(measured_yaw_angle > 6000 && measured_yaw_angle < 8191) {
            measured_yaw_angle = measured_yaw_angle - 6000;
        } else {
            measured_yaw_angle = measured_yaw_angle + 2190;
        }
    }

    /************ PITCH ************/
    // pitch angle range is [around 3520, around 4500]
    // around 3520 is highest pitch position
    // around 4500 is lowest pitch position
    // data 0 and 1 measure angle

    // data 4 and 5 relates to what current you are tx to motors
    // data 4 and 5 NOT same as current tx value
    // -1000 current value = 27852 (pitch_data4)<<8|(pitch_data5) value
    // -750 current value = 24305 (pitch_data4)<<8|(pitch_data5) value
    // -500 current value = 16630 (pitch_data4)<<8|(pitch_data5) value
    // yaw has same current to (data4<<8)|(data5) conversion
    if(rx_message->StdId == 0x206)
    {
        // construct message from data[0] and data[1]
        int16_t pitch_data0 = rx_message->Data[0];
        int16_t pitch_data1 = rx_message->Data[1];
        int16_t pitch_data2 = rx_message->Data[2];
        int16_t pitch_data3 = rx_message->Data[3];
        int16_t pitch_data4 = rx_message->Data[4];
        int16_t pitch_data5 = rx_message->Data[5];

        measured_pitch_angle = (pitch_data0)<<8|(pitch_data1);
        measured_pitch_current = (pitch_data2)<<8|(pitch_data3);
        target_pitch_current = (pitch_data4)<<8|(pitch_data5);
    }
}


/*************************************************************************
                          Code to Set Motor Current Values
Description: Motor_Current_Send(int Motor_ID, int current)
*************************************************************************/

// global variables to store current state in every motor
int16_t motor_yaw_cur;
int16_t motor_pitch_cur;
int16_t motor_front_right_cur;
int16_t motor_front_left_cur;
int16_t motor_back_left_cur;
int16_t motor_back_right_cur;

// different CAN messages for pitch/yaw and wheel motors
CanTxMsg tx_pitchyaw_message;
CanTxMsg tx_wheels_message;

// sets up pitch and yaw address for tx
void PitchYaw_Address_Setup() {
    tx_pitchyaw_message.StdId = 0x1FF;
    tx_pitchyaw_message.DLC = 0x08;
    tx_pitchyaw_message.RTR = CAN_RTR_Data;
    tx_pitchyaw_message.IDE = CAN_Id_Standard;
}

// sets up wheel address for tx
void Wheels_Address_Setup() {
    tx_wheels_message.StdId = 0x200;
    tx_wheels_message.DLC = 0x08;
    tx_wheels_message.RTR = CAN_RTR_Data;
    tx_wheels_message.IDE = CAN_Id_Standard;
}

// prepares whole 0x1FF pitch/yaw CAN message for tx
/*  From Beck's observation, the sending data is the opposite way, big-endian
    first data is top, and that is tested
*/
void Set_PitchYaw_Current() {
    // tx_pitchyaw_message.Data[0] = motor_yaw_cur & 0xFF; // sample out the top 8 bits
    // tx_pitchyaw_message.Data[1] = motor_yaw_cur >> 8;
    // tx_pitchyaw_message.Data[2] = motor_pitch_cur & 0xFF;
    // tx_pitchyaw_message.Data[3] = motor_pitch_cur >> 8;
    tx_pitchyaw_message.Data[0] = motor_yaw_cur >> 8;
    tx_pitchyaw_message.Data[1] = motor_yaw_cur & 0xFF;
    tx_pitchyaw_message.Data[2] = motor_pitch_cur >> 8;
    tx_pitchyaw_message.Data[3] = motor_pitch_cur & 0xFF;
    tx_pitchyaw_message.Data[4] = 0x00;
    tx_pitchyaw_message.Data[5] = 0x00;
    tx_pitchyaw_message.Data[6] = 0x00;
    tx_pitchyaw_message.Data[7] = 0x00;
}

/************ For Red C Motor
// prepares whole 0x200 wheel CAN message for tx
// */
// void Set_Wheels_Current() {
//     tx_wheels_message.Data[0] = motor_front_left_cur >> 8;
//     tx_wheels_message.Data[1] = motor_front_left_cur & 0xFF;
//     tx_wheels_message.Data[2] = motor_back_left_cur >> 8;
//     tx_wheels_message.Data[3] = motor_back_left_cur & 0xFF;
//     tx_wheels_message.Data[4] = motor_front_right_cur >> 8;
//     tx_wheels_message.Data[5] = motor_front_right_cur & 0xFF;
//     tx_wheels_message.Data[6] = motor_back_right_cur >> 8;
//     tx_wheels_message.Data[7] = motor_back_right_cur & 0xFF;
// }


//*********** For Blue Motor
// prepares whole 0x200 wheel CAN message for tx
/*
Change, same as what Beck observed for pitch/yaw
*/
void Set_Wheels_Current() {

    tx_wheels_message.Data[0] = motor_front_right_cur >> 8;
    tx_wheels_message.Data[1] = motor_front_right_cur & 0xFF;
    tx_wheels_message.Data[2] = motor_front_left_cur >> 8;
    tx_wheels_message.Data[3] = motor_front_left_cur & 0xFF;
    tx_wheels_message.Data[4] = motor_back_left_cur >> 8;
    tx_wheels_message.Data[5] = motor_back_left_cur & 0xFF;
    tx_wheels_message.Data[6] = motor_back_right_cur >> 8;
    tx_wheels_message.Data[7] = motor_back_right_cur & 0xFF;
}



// controls pitch and yaw using given currents
void pitchyaw_control(int16_t yaw_current, int16_t pitch_current) {

  PitchYaw_Address_Setup();
  motor_yaw_cur = yaw_current;
  motor_pitch_cur = pitch_current;
  Set_PitchYaw_Current();
  CAN_Transmit(CAN2,&tx_pitchyaw_message);
}

// controls wheels using kinematic equations
void wheel_control(int16_t drive, int16_t strafe, int16_t rotate)
{
    int16_t motor_201_pos = (-1*drive + strafe + rotate);
    int16_t motor_204_pos = (-1*drive - strafe + rotate);
    int16_t motor_202_pos = (drive + strafe + rotate);
    int16_t motor_203_pos = (drive - strafe + rotate);

    // motor_front_right_cur = 11 * motor_201_pos;
    // motor_front_left_cur  = 11 * motor_202_pos;
    // motor_back_left_cur   = 11 * motor_203_pos;
    // motor_back_right_cur  = 11 * motor_204_pos;
    motor_front_right_cur = 6 * motor_201_pos;
    motor_back_right_cur = 6 * motor_204_pos;
    motor_front_left_cur = 6 * motor_202_pos;
    motor_back_left_cur = 6 * motor_203_pos;
    // Velocity_Control_201(motor_201_pos);
    // Velocity_Control_204(motor_204_pos);
    // Velocity_Control_202(motor_202_pos);
    // Velocity_Control_203(motor_203_pos);

    Wheels_Address_Setup();
    Set_Wheels_Current();
    CAN_Transmit(CAN2, &tx_wheels_message);
}



// turn wheels, pitch and yaw
// tx_message1 for wheels
// tx_message2 for pitch and yaw
void Motor_Reset_Can_2(void) {


    CanTxMsg tx_message1;
    CanTxMsg tx_message2;
    motor_yaw_cur = 0;
    motor_pitch_cur = 0;
    motor_front_right_cur = 0;
    motor_front_left_cur = 0;
    motor_back_left_cur = 0;
    motor_back_right_cur = 0;



    tx_message1.StdId = 0x200;
    tx_message1.DLC = 0x08;
    tx_message1.RTR = CAN_RTR_Data;
    tx_message1.IDE = CAN_Id_Standard;

    tx_message2.StdId = 0x1FF;
    tx_message2.DLC = 0x08;
    tx_message2.RTR = CAN_RTR_Data;
    tx_message2.IDE = CAN_Id_Standard;

/*****************************
    tx_message1 Controls wheels
*******************************/

/*
    ************** For Red C Motor **************
    Data 0 and 1 -> Front left wheel            Motor_ID 3
    Data 2 and 3 -> back left wheel             Motor_ID 4
    Data 4 and 5 -> front right wheel           Motor_ID 5
    Data 6 and 7 -> back right wheel            Motor_ID 6

    ************** For Blue C Motor **************
    Data 0 and 1 -> Front right wheel           Motor_ID 3
    Data 2 and 3 -> Front left wheel            Motor_ID 4
    Data 4 and 5 -> Rear left wheel             Motor_ID 5
    Data 6 and 7 -> Rear right wheel            Motor_ID 6


    data is sent in little endian
    positive values -> counter clockwise rotation
    negative values -> clockwise rotation
    I tested:
    +500 to right front wheel. +500 = 0xF401z in little endian so:
    tx_message1.Data[0] = 0xF4;
    tx_message1.Data[1] = 0x01;
    -500 to right front wheel. -500 = 0x0CFE in little endian so::
    tx_message1.Data[0] = 0x0C;
    tx_message1.Data[1] = 0xFE;
*/
    tx_message1.Data[0] = 0x00;
    tx_message1.Data[1] = 0x00;
    tx_message1.Data[2] = 0x00;
    tx_message1.Data[3] = 0x00;
    tx_message1.Data[4] = 0x00;
    tx_message1.Data[5] = 0x00;
    tx_message1.Data[6] = 0x00;
    tx_message1.Data[7] = 0x00;


/*****************************
    tx_message2 Controls pitch and yaw
*******************************/

/*

    tx_message.StdId = 0x1FF for pitch and yaw
    Data 0 and 1 -> yaw (side to side)
    Data 2 and 3 -> pitch (up, down)

    data is sent in little endian
    positive values -> yaw right turn        pitch up
    negative values -> yaw left turn        pitch down
    +1000 to yaw. +1000 = 0xE803 in little endian so:
    tx_message1.Data[0] = 0xE8;
    tx_message1.Data[1] = 0x03;
    -1000 to yaw. -1000 = 0x18FC in little endian so::
    tx_message1.Data[0] = 0x18;
    tx_message1.Data[1] = 0xFC;
*/
    tx_message2.Data[0] = 0x00;
    tx_message2.Data[1] = 0x00;
    tx_message2.Data[2] = 0x00;
    tx_message2.Data[3] = 0x00;
    tx_message2.Data[4] = 0x00;
    tx_message2.Data[5] = 0x00;
    tx_message2.Data[6] = 0x00;
    tx_message2.Data[7] = 0x00;

    CAN_Transmit(CAN2,&tx_message1);
    CAN_Transmit(CAN2,&tx_message2);
}


// For manually setting currents to motors for testing
void Motor_ManSet_Can_2(void) {


    CanTxMsg tx_message1;
    CanTxMsg tx_message2;

    tx_message1.StdId = 0x200;
    tx_message1.DLC = 0x08;
    tx_message1.RTR = CAN_RTR_Data;
    tx_message1.IDE = CAN_Id_Standard;

    tx_message2.StdId = 0x1FF;
    tx_message2.DLC = 0x08;
    tx_message2.RTR = CAN_RTR_Data;
    tx_message2.IDE = CAN_Id_Standard;

/*****************************
    tx_message1 Controls wheels
*******************************/

/*
    tx_message.StdId = 0x200 for wheels

    ************** For Red C Motor **************
    Data 0 and 1 -> Front left wheel            Motor_ID 3
    Data 2 and 3 -> back left wheel             Motor_ID 4
    Data 4 and 5 -> front right wheel           Motor_ID 5
    Data 6 and 7 -> back right wheel            Motor_ID 6

    ************** For Blue C Motor **************
    Data 0 and 1 -> Front right wheel           Motor_ID 3
    Data 2 and 3 -> Front left wheel            Motor_ID 4
    Data 4 and 5 -> Rear left wheel             Motor_ID 5
    Data 6 and 7 -> Rear right wheel            Motor_ID 6

    data is sent in little endian
    positive values -> counter clockwise rotation
    negative values -> clockwise rotation
    I tested:
    +500 to right front wheel. +500 = 0xF401z in little endian so:
    tx_message1.Data[0] = 0xF4;
    tx_message1.Data[1] = 0x01;
    -500 to right front wheel. -500 = 0x0CFE in little endian so::
    tx_message1.Data[0] = 0x0C;
    tx_message1.Data[1] = 0xFE;
*/
    tx_message1.Data[0] = 0xF4;
    tx_message1.Data[1] = 0x01;

    tx_message1.Data[2] = 0xF4;
    tx_message1.Data[3] = 0x01;

    tx_message1.Data[4] = 0x0C;
    tx_message1.Data[5] = 0xFE;

    tx_message1.Data[6] = 0x0C;
    tx_message1.Data[7] = 0xFE;


/*****************************
    tx_message2 Controls pitch and yaw
*******************************/

/*

    tx_message.StdId = 0x1FF for pitch and yaw
    Data 0 and 1 -> yaw (side to side)
    Data 2 and 3 -> pitch (up, down)

    data is sent in little endian
    positive values -> yaw right turn        pitch up
    negative values -> yaw left turn        pitch down
    +1000 to yaw. +1000 = 0xE803 in little endian so:
    tx_message1.Data[0] = 0xE8;
    tx_message1.Data[1] = 0x03;
    -1000 to yaw. -1000 = 0x18FC in little endian so::
    tx_message1.Data[0] = 0x18;
    tx_message1.Data[1] = 0xFC;
*/
    tx_message2.Data[0] = 0x00;
    tx_message2.Data[1] = 0x00;
    tx_message2.Data[2] = 0x00;
    tx_message2.Data[3] = 0x00;
    tx_message2.Data[4] = 0x00;
    tx_message2.Data[5] = 0x00;
    tx_message2.Data[6] = 0x00;
    tx_message2.Data[7] = 0x00;

    // CAN_Transmit(CAN2,&tx_message1);
}

void Encoder_sent(float encoder_angle)
{
    CanTxMsg tx_message;

    tx_message.StdId = 0x601;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;

    encoder_angle = encoder_angle * 100.0f;
    tx_message.Data[0] = (uint8_t)((int32_t)encoder_angle >>24);
    tx_message.Data[1] = (uint8_t)((int32_t)encoder_angle >>16);
    tx_message.Data[2] = (uint8_t)((int32_t)encoder_angle >>8);
    tx_message.Data[3] = (uint8_t)((int32_t)encoder_angle);
    tx_message.Data[4] = 0x00;
    tx_message.Data[5] = 0x00;
    tx_message.Data[6] = 0x00;
    tx_message.Data[7] = 0x00;

    CAN_Transmit(CAN2,&tx_message);
}
