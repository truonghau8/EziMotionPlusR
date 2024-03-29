#include "EziMotionPlusR.h"

bool EziMotionPlusR::encode(char *des, const char *source, const uint8_t length) //length of source
{
	//char *buffer = (char*)malloc(length + 6);
	char *buffer = (char *)malloc(length + 6);

	memcpy((void *)buffer, (const void *)header, 2);
	memcpy((void *)&buffer[2], (const void *)source, (size_t)length);
	this->data.uShort = CalcCRC((unsigned char *)source, length);
	memcpy((void *)&buffer[2 + length], (const void *)data.uChar, 2);
	memcpy((void *)&buffer[4 + length], (const void *)tail, 2);
	memcpy((void *)des, (const void *)buffer, length + 6);
	free(buffer);
	return true;
}

bool EziMotionPlusR::decode(char *des, const char *source, const uint8_t length)
{

	if (length < 4)
		return false;
	if (length >= 256)
		return false;
	// //
	// 	for(int i =0; i< 10; i++)
	// 	{
	// 		Serial.println(source[i], HEX);
	// 	}
	// //
	if ((source[0] == header[0]) || (source[1] == header[1])) //notice
	{

		if (source[length - 1] == tail[1] || source[length - 2] == tail[0]) //notice
		{
			//maybe dynamic
			char *buffer = (char *)malloc(length - 4);
			//
			memcpy((void *)buffer, (const void *)&source[2], length - 4);
			if (checkCRC(buffer, length - 4))
			{
				//des = (char*)malloc(length - 6);
				memcpy((void *)des, (const void *)buffer, length - 6);
				free(buffer);
				return true;
			}
			else
			{
				free(buffer);//moi fix
			}
		}
	}
	return false;
}

int EziMotionPlusR::timeRead()
{
	int c;
	_startMillis = millis();
	do
	{
		if (this->device_model == _HardwareSerial)
		{
			c = this->hwrSerial->read(); /* code */
		}
		if (c >= 0)
			return c;
	} while (millis() - _startMillis < _timeout);
	return -1; // -1 indicates timeout
}



bool EziMotionPlusR::processData(char *des, const char *source, const uint8_t length)
{
	if (length <= 6)
		return false;
	if (des == nullptr)
		return false;
	if (source == nullptr)
		return false;

	switch (this->device_model)
	{
	case _HardwareSerial:
	{
		this->hwrSerial->write(source, length);
		//

		char buffer[256];
		int c = this->timeRead();
		char _length_buffer = 0;
		while (c >= 0)
		{
			buffer[_length_buffer++] = (char)c;
			c = this->timeRead();

		}
		memcpy((void *)des, (const void *)buffer, (size_t)(_length_buffer - 1));
		return true;
		//
		break;
	}
	default:
		break;
	}
	return false;
}
bool EziMotionPlusR::processData(char *des, const char *source, const uint8_t length, const uint8_t length_des)
{
	if (length <= 6)
		return false;
	if (des == nullptr)
		return false;
	if (source == nullptr)
		return false;

	switch (this->device_model)
	{
	case _HardwareSerial:
	{
		uint8_t length_written = 0;
		unsigned long last_time_write = millis();
		do
		{
			length_written = (uint8_t)this->hwrSerial->write(source, length);
			if (millis() - last_time_write > 1000)
			{
				return false;
				//break;
			}
		} while (length_written < length);

		float time_delay = (float)((1.0/this->dwbaud)*(length + length_des)*8*1000000)*2.0;
		// Serial.println();
		// Serial.println(time_delay);

		delayMicroseconds(time_delay);
		//
		//Serial.println(source);

		char buffer[256];
		int c = this->timeRead();
		char _length_buffer = 0;
		while (c >= 0)
		{
			buffer[_length_buffer++] = (char)c;
			c = this->timeRead();
			if(_length_buffer >255)
			{
				break;
			}
		}
		memcpy((void *)des, (const void *)buffer, length_des);
		return true;
		//
		break;
	}
	default:
		break;
	}
	return false;
}

bool EziMotionPlusR::checkCRC(char *source, const uint8_t length)
{
	unsigned short CRC_value = CalcCRC((unsigned char *)source, (unsigned long)length - 2);
	data.uChar[0] = source[length - 2];
	data.uChar[1] = source[length - 1];

	if ((data.uShort ^ CRC_value) == 0x0000)
	{
		return true;
	}
	return false;
}

EziMotionPlusR::EziMotionPlusR(HardwareSerial &uart_ttl, int32_t dwbaud)
{
	this->hwrSerial = &uart_ttl;
	this->device_model = _HardwareSerial;
	this->dwbaud = dwbaud;
	//this->iSlaveNo = iSlaveNo;
}

EziMotionPlusR::~EziMotionPlusR()
{
}
bool EziMotionPlusR::initServo()
{
	switch (this->device_model)
	{
	case _HardwareSerial:
		this->hwrSerial->begin((unsigned long)this->dwbaud);
		delay(50);
		if (this->hwrSerial)
			return true;
		break;
	default:
		break;
	}
	return false;
}
char EziMotionPlusR::ServoEnable(uint8_t iSlaveNo, bool state)
{
	char *buffer = (char *)malloc(4 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_ServoEnable;
	buffer[3] = (char)state;
	this->encode(buffer, (const char *)buffer, 4);

	//////////////////////////////////////////////////////////
	//
	//---Send data encoded to servo and wait respond
	//
	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(4 + 6), 10);
	free(buffer);

	//buffer = (char*)malloc(6 + 4);
	//
	//recive
	//
	/////////////////////////////////////////////////////////////

	//
	//decode
	char *buffer_out = (char *)malloc(4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)10);
	//
	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
	//if (data == CommIsNormal)
	//check if data is
}
char EziMotionPlusR::ServoAlarmReset(uint8_t iSlaveNo)
{
	char *buffer = (char *)malloc(3 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_ServoAlarmReset;
	this->encode(buffer, (const char *)buffer, 3);

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 6), 10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)10);

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}
char EziMotionPlusR::MoveStop(uint8_t iSlaveNo)
{
	char *buffer = (char *)malloc(3 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_MoveStop;
	this->encode(buffer, (const char *)buffer, 3);

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 6), 10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;

	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)10);

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::EmergencyStop(uint8_t iSlaveNo)
{
	char *buffer = (char *)malloc(3 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_EmergencyStop;
	this->encode(buffer, (const char *)buffer, 3);

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 6), 10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)10);

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}
char EziMotionPlusR::MoveOriginSingleAxis(uint8_t iSlaveNo)
{
	char *buffer = (char *)malloc(3 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_MoveOriginSingleAxis;
	this->encode(buffer, (const char *)buffer, 3);

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 6), 10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);

	bool result = false;

	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)10);

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::MoveSingleAxisAbsPos(uint8_t iSlaveNo, int32_t posVal, uint32_t speed_pps)
{
	char *buffer = (char *)malloc(3 + 8 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_MoveSingleAxisAbsPos;
	buffer[3] = (char)posVal;
	buffer[4] = (char)(posVal >> 8);
	buffer[5] = (char)(posVal >> 16);
	buffer[6] = (char)(posVal >> 24);
	buffer[7] = (char)speed_pps;
	buffer[8] = (char)(speed_pps >> 8);
	buffer[9] = (char)(speed_pps >> 16);
	buffer[10] = (char)(speed_pps >> 24);

	// Serial.println("thu nghiem");
	// for(uint8_t i = 0; i< 11; i++)
	// {
	// 	Serial.println(buffer[i], HEX);
	// }

	this->encode(buffer, (const char *)buffer, 3 + 8);

	// Serial.println("testaaaa::aa");
	// for(int8_t i = 0; i < 17; i++)
	// {
	// 	Serial.println(buffer[i], HEX);
	// }

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 8 + 6), 10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;

	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(4 + 6));

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::MoveSingleAxisIncPos(uint8_t iSlaveNo, int32_t posVal, uint32_t speed_pps)
{
	char *buffer = (char *)malloc(3 + 8 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_MoveSingleAxisIncPos;
	buffer[3] = (char)posVal;
	buffer[4] = (char)(posVal >> 8);
	buffer[5] = (char)(posVal >> 16);
	buffer[6] = (char)(posVal >> 24);
	buffer[7] = (char)speed_pps;
	buffer[8] = (char)(speed_pps >> 8);
	buffer[9] = (char)(speed_pps >> 16);
	buffer[10] = (char)(speed_pps >> 24);

	// Serial.println("thu nghiem");
	// for(uint8_t i = 0; i< 11; i++)
	// {
	// 	Serial.println(buffer[i], HEX);
	// }

	this->encode(buffer, (const char *)buffer, 3 + 8);

	// Serial.println("testaaaa::aa");
	// for(int8_t i = 0; i < 17; i++)
	// {
	// 	Serial.println(buffer[i], HEX);
	// }

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 8 + 6), 10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;

	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(4 + 6));

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::MoveVelocity(uint8_t iSlaveNo, uint32_t speed_pps, bool jog_dir)
{
	char *buffer = (char *)malloc(3 + 5 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_MoveVelocity;
	buffer[3] = (char)speed_pps;
	buffer[4] = (char)(speed_pps >> 8);
	buffer[5] = (char)(speed_pps >> 16);
	buffer[6] = (char)(speed_pps >> 24);
	buffer[7] = (char)jog_dir;
	// Serial.println("thu nghiem");
	// for(uint8_t i = 0; i< 11; i++)
	// {
	// 	Serial.println(buffer[i], HEX);
	// }

	this->encode(buffer, (const char *)buffer, 3 + 5);

	// Serial.println("testaaaa::aa");
	// for(int8_t i = 0; i < 17; i++)
	// {
	// 	Serial.println(buffer[i], HEX);
	// }

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 5 + 6), (const uint8_t)10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;

	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(4 + 6));

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::GetCommandPos(uint8_t iSlaveNo, int32_t *cmdPosValueRt)
{
	char *buffer = (char *)malloc(3 + 5 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_GetCommandPos;

	this->encode(buffer, (const char *)buffer, 3);

	char *buffer_respond = (char *)malloc(4 + 4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 6), 14);
	free(buffer);

	char *buffer_out = (char *)malloc(4 + 4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(3 + 5 + 6));

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)cmdPosValueRt, (const void *)&buffer_out[4], 4);
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::SetCommandPos(uint8_t iSlaveNo, int32_t cmdPosValueRt)
{
	char *buffer = (char *)malloc(3 + 4 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_SetCommandPos;
	buffer[3] = (char)cmdPosValueRt;
	buffer[4] = (char)(cmdPosValueRt >> 8);
	buffer[5] = (char)(cmdPosValueRt >> 16);
	buffer[6] = (char)(cmdPosValueRt >> 24);
	this->encode(buffer, (const char *)buffer, 3 + 4);

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 4 + 6));
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(3 + 1 + 6));

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::GetActualPos(uint8_t iSlaveNo, int32_t *cmdPosValueRt)
{
	char *buffer = (char *)malloc(3 + 5 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_GetActualPos;

	this->encode(buffer, (const char *)buffer, 3);

	char *buffer_respond = (char *)malloc(4 + 4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 6), 14);
	free(buffer);

	char *buffer_out = (char *)malloc(4 + 4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(3 + 5 + 6));
	if (!result)
	{
		//Serial.println("err data");
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}
	char data;
	memcpy((void *)cmdPosValueRt, (const void *)&buffer_out[4], 4);
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::SetActualPos(uint8_t iSlaveNo, int32_t cmdPosValueRt)
{
	char *buffer = (char *)malloc(3 + 4 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_SetActualPos;
	buffer[3] = (char)cmdPosValueRt;
	buffer[4] = (char)(cmdPosValueRt >> 8);
	buffer[5] = (char)(cmdPosValueRt >> 16);
	buffer[6] = (char)(cmdPosValueRt >> 24);
	this->encode(buffer, (const char *)buffer, 3 + 4);

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 4 + 6));
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(3 + 1 + 6));

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::ClearPosition(uint8_t iSlaveNo)
{
	char *buffer = (char *)malloc(3 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_ClearPosition;
	this->encode(buffer, (const char *)buffer, 3);

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(4 + 6), 10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)10);

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::SetParameter(uint8_t iSlaveNo, uint8_t para_num, int32_t data_para)
{
	char *buffer = (char *)malloc(3 + 5 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_SetParameter;
	buffer[3] = (char)para_num;
	buffer[4] = (char)data_para;
	buffer[5] = (char)(data_para >> 8);
	buffer[6] = (char)(data_para >> 16);
	buffer[7] = (char)(data_para >> 24);

	// Serial.println("thu nghiem");
	// for(uint8_t i = 0; i< 11; i++)
	// {
	// 	Serial.println(buffer[i], HEX);
	// }

	this->encode(buffer, (const char *)buffer, 3 + 5);

	// Serial.println("testaaaa::aa");
	// for(int8_t i = 0; i < 17; i++)
	// {
	// 	Serial.println(buffer[i], HEX);
	// }

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 5 + 6), 10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(3 + 1 + 6));

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::GetParameter(uint8_t iSlaveNo, uint8_t para_num, int32_t *data_para)
{
	char *buffer = (char *)malloc(3 + 5 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_GetParameter;
	buffer[3] = (char)para_num;

	this->encode(buffer, (const char *)buffer, 4);

	char *buffer_respond = (char *)malloc(4 + 4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 6), 14);
	free(buffer);

	char *buffer_out = (char *)malloc(4 + 4);
	bool result = false;

	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(3 + 5 + 6));

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)data_para, (const void *)&buffer_out[4], 4);
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::VelocityOverride(uint8_t iSlaveNo, uint32_t new_speed_cmd)
{
	char *buffer = (char *)malloc(3 + 4 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_VelocityOverride;
	buffer[3] = (char)new_speed_cmd;
	buffer[4] = (char)(new_speed_cmd >> 8);
	buffer[5] = (char)(new_speed_cmd >> 16);
	buffer[6] = (char)(new_speed_cmd >> 24);
	this->encode(buffer, (const char *)buffer, 3 + 4);

	char *buffer_respond = (char *)malloc(4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 4 + 6), 10);
	free(buffer);

	char *buffer_out = (char *)malloc(4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(3 + 1 + 6));

	if (!result)
	{
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}

	char data;
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}

char EziMotionPlusR::GetIOOutput(uint8_t iSlaveNo, int32_t *data_para)
{	

	char *buffer = (char *)malloc(3 + 5 + 6);
	buffer[0] = iSlaveNo;
	buffer[1] = this->value_random++;
	buffer[2] = FAS_GetIOOutput;

	this->encode(buffer, (const char *)buffer, 3);

	char *buffer_respond = (char *)malloc(4 + 4 + 6);
	this->processData(buffer_respond, (const char *)buffer, (const uint8_t)(3 + 6), 14);
	free(buffer);

	char *buffer_out = (char *)malloc(4 + 4);
	bool result = false;
	result = this->decode(buffer_out, (const char *)buffer_respond, (const uint8_t)(3 + 5 + 6));

	if (!result)
	{
		//Serial.println("err data");
		free(buffer_respond);
		free(buffer_out);
		return CRC_err;
	}
	char data;
	memcpy((void *)data_para, (const void *)&buffer_out[4], 4);
	memcpy((void *)&data, (const void *)&buffer_out[3], 1);
	free(buffer_respond);
	free(buffer_out);
	return data;
}