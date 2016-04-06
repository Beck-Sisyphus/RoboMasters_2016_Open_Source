#ifndef __APP_H__
#define __APP_H__
#include <stm32f4xx.h>
#include "map.h"

#define ESC_MAX 50000.0
#define GAP 0.0
#define abs(x) ((x)>0? (x):(-(x)))

// Measured reading
#define REAL_PITCH_LOW 56  // real lowest angle value that the cannon can reach, measured by protractor
#define REAL_PITCH_HIGH 114 // real lowest angle value that the cannon can reach, measured by protractor
#define REAL_YAW_LOW -103
#define REAL_YAW_HIGH 103
// Encoder reading
#define BLUE_PITCH_LOW 4789
#define BLUE_PITCH_HIGH 3544
#define BLUE_YAW_LOW 37
#define BLUE_YAW_HIGH 4803

void set_Pitch_Position(uint16_t target_pitch_angle);
void set_Yaw_Position(uint16_t target_yaw_angle);
void Cmd_ESC(int16_t current_201,int16_t current_202,int16_t current_203);

float Velocity_Control_205(float current_velocity_201,float target_velocity_201);
float Position_Control_205(float current_position_201,float target_position_201);

float Velocity_Control_206(float current_velocity_203,float target_velocity_203);
float Position_Control_206(float current_position_203,float target_position_203);

#endif
