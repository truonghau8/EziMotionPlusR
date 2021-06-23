#include "EziMotionPlusR.h"




bool EziMotionPlusR::encode(char * des, const char * source, const int8_t length) //length of source
{
	//char *buffer = (char*)malloc(length + 6);
	char *buffer = (char*)malloc(length + 6);

	memcpy((void*)buffer, (const void*)header, 2);
	memcpy((void*)&buffer[2], (const void*)source, (size_t)length);
	data.uShort = CalcCRC((unsigned char*)source, length);
	memcpy((void*)&buffer[2 + length], (const void*)data.uChar, 2);
	memcpy((void*)&buffer[4 + length], (const void*)tail, 2);
	memcpy((void*)des, (const void*)buffer, length + 6);

	return true;
}

bool EziMotionPlusR::decode(char * des, const char* source, const int8_t length)
{
	if (length < 4) return false;
	if (length >= 256) return false;

	

	if ((source[0] == header[0]) && (source[1] == header[1]))
	{
		if (source[length - 1] == tail[1] && source[length - 2] == tail[0])
		{
			//maybe dynamic
			char *buffer = (char*)malloc(length - 4);
			//
			memcpy((void*)buffer, (const void*)&source[2], length - 4);
			if (checkCRC(buffer, length - 4))
			{
				//des = (char*)malloc(length - 6);
				memcpy((void*)des, (const void*)buffer, length - 6);

				return true;
			}
		}
	}
	return false;
}

bool EziMotionPlusR::checkCRC(char * source, const int8_t length)
{
	unsigned short CRC_value = CalcCRC((unsigned char*)source, (unsigned long)length - 2);
	data.uChar[0] = source[length - 2];
	data.uChar[1] = source[length - 1];

	if ((data.uShort^CRC_value) == 0x0000)
	{
		return true;
	}
	return false;
}


EziMotionPlusR::EziMotionPlusR(int32_t dwbaud, int8_t iSlaveNo)
{
	this->dwbaud = dwbaud;
	this->iSlaveNo = iSlaveNo;
}

EziMotionPlusR::~EziMotionPlusR()
{
}
bool EziMotionPlusR::initServo()
{

	return false;
}
int8_t EziMotionPlusR::ServoEnable(int8_t iSlaveNo, bool state)
{
	char *buffer = (char*)malloc(4 + 6);
	buffer[0] = this->iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_ServoEnable;
	buffer[3] = (char)state;
	this->encode(buffer, (const char*)buffer, 4);
	//
	//---Send data encoded to servo and wait respond
	//
	free(buffer);

	//buffer = (char*)malloc(6 + 4);
	//
	//recive
	//



	char buffer_fake[] = {0xAA, 0xCC, 0x00, 0x00, 0x2A, 0x00, 0x1F, 0x44, 0xAA, 0xEE};
	//
	//decode
	char *buffer_out = (char*)malloc(4);
	this->decode(buffer_out, (const char*)buffer_fake, 6 + 4);
	//
	uint8_t data;
	memcpy((void*)&data, (const void*)&buffer_out[3], 1);
	free(buffer_out);
	return data;
	//if (data == CommIsNormal)
	//check if data is 
}
