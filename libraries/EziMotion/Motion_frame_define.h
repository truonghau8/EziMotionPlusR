#pragma once

#define FAS_GetSalveInfo			0x01
#define FAS_GetMotorInfo			0x05
#define FAS_SaveAllParameters		0x10
#define FAS_GetRomParameter			0x11
#define FAS_SetParameter			0x12
#define FAS_GetParameter			0x13
#define FAS_SetIOOutput				0x20
#define FAS_SsetIOInput				0x21
#define FAS_GetIOInput				0x22
#define FAS_GetIOOutput				0x23
#define FAS_SetIOAssignMap			0x24
#define FAS_GetIOAssignMap			0x25
#define FAS_IOAssignMapReadROM		0x26
#define FAS_TriggerOutput_RunA		0x27
#define FAS_TriggerOutput_Status	0x28
#define FAS_ServoEnable				0x2A //function
#define FAS_ServoAlarmReset			0x2B //function
#define FAS_ServoAlarmtype			0x2E 
#define FAS_MoveStop                0x31 //function
#define FAS_EmergencyStop           0x32 //function
#define FAS_MoveOriginSingleAxis    0x33 //function
#define FAS_MoveSingleAxisAbsPos    0x34 //function
#define FAS_MoveSingleAxisIncPos    0x35 //function
#define FAS_MoveToLimit             0x36
#define FAS_MoveVelocity            0x37 //need function
#define FAS_PositionAbsOverride     0x38 //need function
#define FAS_PositionIncOverride     0x39 //need function
#define FAS_VelocityOverride        0x3A //need function
#define FAS_AllMoveStop             0x3B //need function
#define FAS_AllEmergencyStop        0x3C //need function
#define FAS_MoveOriginSingleAxis    0x3D
#define FAS_SingleAxisAbsPos        0x3E
#define FAS_AllSingleAxisIncPos     0x3F
#define FAS_MoveSingleAxisAbsPosEx	0x80 //need function
#define FAS_MoveSingleAxisIncPosEx	0x81 //need function
#define FAS_MoveVelocityEX			0x82
#define FAS_GetAxisStatus			0x40 //function
#define FAS_GetIOAxisStatus			0x41 //function
#define FAS_GetMotionStatus			0x42 //function
#define FAS_GetAllStatus			0x43 
#define FAS_SetCommandPos			0x50
#define FAS_GetCommandPos			0x51
#define FAS_SetActualPos			0x52 //function
#define FAS_GetActualPos			0x53 //function
#define FAS_GetPosError				0x54 //function
#define FAS_GetActualVel			0x55 //function
#define FAS_ClearPosition			0x56 //function
#define FAS_MovePause				0x58 //function
#define FAS_PosTableReadItem		0x60
#define FAS_PosTableWriteItem		0x61
#define FAS_PosTableReadRom			0x62
#define FAS_PosTableWriteRom		0x63
#define FAS_POsTableRunItem			0x64
#define FAS_PosTableReadOneItem		0x6A
#define FAS_PosTableWriteOneItem	0x6B
#define FAS_MovePush				0x78
#define FAS_GetPushStatus			0x79
#define FAS_SetParameter 			0x12
#define FAS_GetParameter 			0x13



//parameter table
#define Para_Pulse_per_Revolution 	0x00
#define Para_Axis_Max_Speed 		0x01
#define Para_Axis_Start_Speed		0x02
#define Para_Axis_Acc_Time			0x03
#define Para_Axis_Dec_Time 			0x04
#define Para_Speed_Override			0x05
#define Para_Jog_Speed				0x06
#define Para_Jog_Start_Speed		0x07
#define Para_Jog_Acc_Dec_Time		0x08
#define Para_Servo_Alarm_Logic		0x09
#define Para_Servo_On_Logic			0x0A
#define Para_Servo_Alarm_Reset_Logic 11
#define Para_SW_Limit_Plus_Value	12
#define Para_SW_Limit_Minus_Value 	13
#define Para_SW_Limit_Stop_Method	14
#define Para_HW_Limit_Stop_Method	15
#define Para_Limit_Sensor_Logic		16
#define Para_Org_Speed				17
#define Para_Org_Search_Speed		18
#define Para_Org_Acc_Dec_Time		19
#define Para_Org_Method				20
#define Para_Org_Dir				21
#define Para_Org_Offset				22
#define Para_Org_Position_Set		23
#define Para_Org_Sensor_Logic		24
#define Para_Position_Loop_Gain		25
#define Para_Inpos_Value			26
#define Para_Pos_Tracking_Limit		27
#define Para_Motion_Dir				28
#define Para_Limit_Sensor_Dir		29
#define Para_Org_Torque_Ratio		30
#define Para_Pos_err_overflow_Limit	31
#define Para_Pos_val_counting_method	32
#define Para_Servo_on_method		33
#define Para_Brake_delay_time		34
#define Para_reserved				35
#define Para_Run_Current			36
#define Para_Stop_Current			37
#define Para_Boost_Current			38
//

enum AlarmType
{
	No_Alarm = 0,
	OverCurrent,
	OverSpeed,
	StepOut,
	OverLoad,
	OverTemperature,
	BackEMF,
	MotorConnect,
	EncoderConnect,
	MotorPower,
	Inpostiton,
	SystemHalt,
	ROMdevice,
	OverInputVoltage,
	PositionOverflow
};