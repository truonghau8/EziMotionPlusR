//This library was written by Hau Truong
//E-mail: truonghau8@gmail.com
//github: https://github.com/truonghau8/

#ifndef _EZIMOTIONPLUS_H
#define _EZIMOTIONPLUS_H

#include <iostream>
#include "CRC_cal.h"
#include "CommStatusEziMotion.h"
#include "Motion_frame_define.h"
#include <string.h>

union split_uShort
{
	unsigned short uShort = 0;
	unsigned char uChar[2];
};
split_uShort data;

class EziMotionPlusR
{
	private:
		char header[2] = {(char)0xAA, (char)0xCC};
		char tail[2] = {(char)0xAA, (char)0xEE };
		char buffer[256] = { 0 };
		int32_t dwbaud = 0;
		int8_t iSlaveNo = 0;
		uint8_t value_random = 0;
		//
		//header source crc tail
		//source
		//source = slave_No + code_id_random + code_function + [data_function]
		//
		bool encode(char *des, const char *source, const int8_t length); //encode_buffer //da test
		bool decode(char *des, const char *source, const int8_t length); //decode_buffer //da test
		bool checkCRC(char *source, const int8_t length); //encode_buffer
	public:
		EziMotionPlusR(int32_t dwbaud, int8_t iSlaveNo);
		~EziMotionPlusR();
		bool initServo();
		int8_t ServoEnable(int8_t iSlaveNo, bool state);
		

};

#endif