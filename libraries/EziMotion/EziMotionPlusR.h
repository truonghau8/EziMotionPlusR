/* Maintainer: Indruino */

#ifndef _EZIMOTIONPLUS_H
#define _EZIMOTIONPLUS_H

//#include <iostream>
#include "Arduino.h"
//#include "SoftwareSerial.h"
#include "CRC_cal.h"
#include "CommStatusEziMotion.h"
#include "Motion_frame_define.h"
#include <string.h>

union split_uShort
{
	unsigned short uShort = 0;
	unsigned char uChar[2];
};
enum SerialModel
{
	_HardwareSerial = 0,
	_SoftwareSerial
};

class EziMotionPlusR
{
	private:
		char header[2] = {(char)0xAA, (char)0xCC};
		char tail[2] = {(char)0xAA, (char)0xEE };
		char buffer[256] = { 0 };
		//choose serial
		uint8_t device_model;
		HardwareSerial *hwrSerial;
		//SoftwareSerial *swrSerial;
		//choose serial
		int32_t dwbaud = 0;
		//uint8_t iSlaveNo = 0;
		uint8_t value_random = 0;

		
		split_uShort data;
		//
		//header source crc tail
		//source
		//source = slave_No + code_id_random + code_function + [data_function]
		//
		unsigned long _startMillis;
		const unsigned short _timeout = 1;

		//bool flag_timeout_PDU = false;

		bool encode(char *des, const char *source, const uint8_t length); //encode_buffer //da test
		bool decode(char *des, const char *source, const uint8_t length); //decode_buffer //da test
		bool processData(char *des, const char *source, const uint8_t length); //send data and after that it wait the data respond
		bool processData(char *des, const char *source, const uint8_t length, const uint8_t length_des); //send data and after that it wait the data respond
		bool checkCRC(char *source, const uint8_t length); //encode_buffer
		int timeRead();
	public:
		EziMotionPlusR(HardwareSerial& uart_ttl,int32_t dwbaud);
		//EziMotionPlusR(SoftwareSerial& uart_ttl,int32_t dwbaud);
		~EziMotionPlusR();
		bool initServo();
		char ServoEnable(uint8_t iSlaveNo, bool state); //da test
		char ServoAlarmReset(uint8_t iSlaveNo); //da test
		char MoveStop(uint8_t iSlaveNo); //da test
		char EmergencyStop(uint8_t iSlaveNo); //da test
		char MoveOriginSingleAxis(uint8_t iSlaveNo); //da test
		char MoveSingleAxisAbsPos(uint8_t iSlaveNo, int32_t posVal, uint32_t speed_pps); //da test
		char MoveSingleAxisIncPos(uint8_t iSlaveNo, int32_t posVal, uint32_t speed_pps); //da test
		char MoveVelocity(uint8_t iSlaveNo, uint32_t speed_pps, bool jog_dir); //da test
		char GetCommandPos(uint8_t iSlaveNo, int32_t *cmdPosValueRt); 
		char SetCommandPos(uint8_t iSlaveNo, int32_t cmdPosValueRt); 
		char GetActualPos(uint8_t iSlaveNo, int32_t *cmdPosValueRt);
		char SetActualPos(uint8_t iSlaveNo, int32_t cmdPosValueRt);
		char ClearPosition(uint8_t iSlaveNo);
		char SetParameter(uint8_t iSlaveNo, uint8_t para_num, int32_t data_para);
		char GetParameter(uint8_t iSlaveNo, uint8_t para_num, int32_t *data_para);

		char VelocityOverride(uint8_t iSlaveNo, uint32_t new_speed_cmd);
		char GetIOOutput(uint8_t iSlaveNo, int32_t *data_para);
		//bool check_timeout_PDU();
		//
		//set para -Acel time -- Decl time
		//
};

#endif