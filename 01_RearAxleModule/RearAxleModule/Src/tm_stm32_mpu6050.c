/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_mpu6050.h"

/* Default I2C address */
#define MPU6050_I2C_ADDR			0xD0

/* Who I am register value */
#define MPU6050_I_AM				0x68

/* MPU6050 registers */
#define MPU6050_AUX_VDDIO			0x01
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG			0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_MOTION_THRESH		0x1F
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
#define MPU6050_INT_STATUS			0x3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_MOT_DETECT_STATUS	0x61
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_MOT_DETECT_CTRL		0x69
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75

/* Gyro sensitivities in degrees/s */
#define MPU6050_GYRO_SENS_250		((double) 131)
#define MPU6050_GYRO_SENS_500		((double) 65.5)
#define MPU6050_GYRO_SENS_1000		((double) 32.8)
#define MPU6050_GYRO_SENS_2000		((double) 16.4)

/* Acce sensitivities in g/s */
#define MPU6050_ACCE_SENS_2			((double) 16384)
#define MPU6050_ACCE_SENS_4			((double) 8192)
#define MPU6050_ACCE_SENS_8			((double) 4096)
#define MPU6050_ACCE_SENS_16		((double) 2048)


TM_MPU6050_Result_t TM_MPU6050_Init(TM_MPU6050_t* DataStruct, TM_MPU6050_Device_t DeviceNumber, TM_MPU6050_Accelerometer_t AccelerometerSensitivity, TM_MPU6050_Gyroscope_t GyroscopeSensitivity, TM_MPU6050_DigitalLowPassFilter_t Bandwidth) {
	uint8_t temp;
	uint8_t d[2];
	
	/* Format I2C address */
	DataStruct->Address = MPU6050_I2C_ADDR | (uint8_t)DeviceNumber;
	
	/* Initialize I2C */
	MX_I2C1_Init();
	
	/* Check if device is connected */
	if(HAL_I2C_IsDeviceReady(&MPU6050_I2C_INSTANCE,DataStruct->Address,2,5) != HAL_OK){
		return TM_MPU6050_Result_DeviceNotConnected;
	}

	/* Check who am I */
	if (HAL_I2C_Mem_Read(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_WHO_AM_I, 1, &temp, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_DeviceInvalid;
	}
	if(temp != MPU6050_I_AM){
		/* Return error */
		return TM_MPU6050_Result_DeviceInvalid;
	}

	/* Wakeup MPU6050 */

	/* Format array to send */
	d[0] = MPU6050_PWR_MGMT_1;
	d[1] = 0x00;
	/* Try to transmit via I2C */
	if (HAL_I2C_Master_Transmit(&MPU6050_I2C_INSTANCE, DataStruct->Address, (uint8_t *)d, 2, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Set sample rate to 1kHz */
	TM_MPU6050_SetDataRate(DataStruct, TM_MPU6050_DataRate_1KHz);
	
	/* Config accelerometer */
	TM_MPU6050_SetAccelerometer(DataStruct, AccelerometerSensitivity);
	
	/* Config accelerometer */
	TM_MPU6050_SetGyroscope(DataStruct, GyroscopeSensitivity);
	
	/* Config Digital Low Pass Filter */
	TM_MPU6050_SetDigitalLowPassFilter(DataStruct, Bandwidth);
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_SetGyroscope(TM_MPU6050_t* DataStruct, TM_MPU6050_Gyroscope_t GyroscopeSensitivity) {
	uint8_t temp;
	
	/* Config gyroscope */
	if (HAL_I2C_Mem_Read(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_GYRO_CONFIG, 1, &temp, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	temp = (temp & 0xE7) | (uint8_t)GyroscopeSensitivity << 3;
	if (HAL_I2C_Mem_Write(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_GYRO_CONFIG, 1, &temp, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	switch (GyroscopeSensitivity) {
		case TM_MPU6050_Gyroscope_250s:
			DataStruct->Gyroscope_Multiply = (double)1 / MPU6050_GYRO_SENS_250;
			break;
		case TM_MPU6050_Gyroscope_500s:
			DataStruct->Gyroscope_Multiply = (double)1 / MPU6050_GYRO_SENS_500;
			break;
		case TM_MPU6050_Gyroscope_1000s:
			DataStruct->Gyroscope_Multiply = (double)1 / MPU6050_GYRO_SENS_1000;
			break;
		case TM_MPU6050_Gyroscope_2000s:
			DataStruct->Gyroscope_Multiply = (double)1 / MPU6050_GYRO_SENS_2000;
		default:
			break;
	}
	
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_SetAccelerometer(TM_MPU6050_t* DataStruct, TM_MPU6050_Accelerometer_t AccelerometerSensitivity) {
	uint8_t temp;
	
	/* Config accelerometer */
	if (HAL_I2C_Mem_Read(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_ACCEL_CONFIG, 1, &temp, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	temp = (temp & 0xE7) | (uint8_t)AccelerometerSensitivity << 3;
	if (HAL_I2C_Mem_Write(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_ACCEL_CONFIG, 1, &temp, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Set sensitivities for multiplying gyro and accelerometer data */
	switch (AccelerometerSensitivity) {
		case TM_MPU6050_Accelerometer_2G:
			DataStruct->Accelerometer_Multiply = (double)1 / MPU6050_ACCE_SENS_2;
			break;
		case TM_MPU6050_Accelerometer_4G:
			DataStruct->Accelerometer_Multiply = (double)1 / MPU6050_ACCE_SENS_4;
			break;
		case TM_MPU6050_Accelerometer_8G:
			DataStruct->Accelerometer_Multiply = (double)1 / MPU6050_ACCE_SENS_8;
			break;
		case TM_MPU6050_Accelerometer_16G:
			DataStruct->Accelerometer_Multiply = (double)1 / MPU6050_ACCE_SENS_16;
		default:
			break;
	}
	
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_SetDataRate(TM_MPU6050_t* DataStruct, uint8_t rate) {
	/* Set data sample rate */
	if (HAL_I2C_Mem_Write(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_SMPLRT_DIV, 1, &rate, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_SetDigitalLowPassFilter(TM_MPU6050_t* DataStruct, TM_MPU6050_DigitalLowPassFilter_t Bandwidth) {
	/* Set Digital Low Pass Filter bandwidth */
	if (HAL_I2C_Mem_Write(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_CONFIG, 1, (uint8_t*) &Bandwidth, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_EnableInterrupts(TM_MPU6050_t* DataStruct) {
	uint8_t temp;
	
	temp = 0x21;
	/* Enable interrupts for data ready and motion detect */
	if (HAL_I2C_Mem_Write(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_INT_ENABLE, 1, &temp, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}

	/* Clear IRQ flag on any read operation */
	if (HAL_I2C_Mem_Read(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_INT_PIN_CFG, 1, &temp, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	temp |= 0x10;
	if (HAL_I2C_Mem_Write(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_INT_PIN_CFG, 1, &temp, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_DisableInterrupts(TM_MPU6050_t* DataStruct) {
	uint8_t temp;
	temp = 0x00;
	/* Disable interrupts */
	if (HAL_I2C_Mem_Write(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_INT_ENABLE, 1, &temp, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}

	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_ReadInterrupts(TM_MPU6050_t* DataStruct, TM_MPU6050_Interrupt_t* InterruptsStruct) {
	uint8_t read;
	
	/* Reset structure */
	InterruptsStruct->Status = 0;
	
	/* Read interrupts status register */
	if (HAL_I2C_Mem_Read(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_INT_STATUS, 1, &read, 1, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Fill value */
	InterruptsStruct->Status = read;
	
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_ReadAccelerometer(TM_MPU6050_t* DataStruct) {
	uint8_t data[6];
	
	/* Read accelerometer data */
	if (HAL_I2C_Mem_Read(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_ACCEL_XOUT_H, 1, data, 6, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Format */
	DataStruct->Accelerometer_Raw.x = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Accelerometer_Raw.y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Accelerometer_Raw.z = (int16_t)(data[4] << 8 | data[5]);
	
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_ReadGyroscope(TM_MPU6050_t* DataStruct) {
	uint8_t data[6];
	
	/* Read gyroscope data */
	if (HAL_I2C_Mem_Read(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_GYRO_XOUT_H, 1, data, 6, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Format */
	DataStruct->Gyroscope_Raw.x = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Gyroscope_Raw.y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Gyroscope_Raw.z = (int16_t)(data[4] << 8 | data[5]);

	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_ReadTemperature(TM_MPU6050_t* DataStruct) {
	uint8_t data[2];
	int16_t temp;
	
	/* Read temperature */
	if (HAL_I2C_Mem_Read(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_TEMP_OUT_H, 1, data, 2, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Format temperature */
	temp = (data[0] << 8 | data[1]);
	DataStruct->Temperature = (double)((int16_t)temp / (double)340.0 + (double)36.53);
	
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_ReadAll(TM_MPU6050_t* DataStruct) {
	uint8_t data[14];
	int16_t temp;
	
	/* Read full raw data, 14bytes */
	if (HAL_I2C_Mem_Read(&MPU6050_I2C_INSTANCE, DataStruct->Address, MPU6050_TEMP_OUT_H, 1, data, 14, 1000) != HAL_OK) {
		/* Return error */
		return TM_MPU6050_Result_Error;
	}
	
	/* Format accelerometer data */
	DataStruct->Accelerometer_Raw.x = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Accelerometer_Raw.y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Accelerometer_Raw.z = (int16_t)(data[4] << 8 | data[5]);

	/* Format temperature */
	temp = (data[6] << 8 | data[7]);
	DataStruct->Temperature = (double)((double)((int16_t)temp) / (double)340.0 + (double)36.53);
	
	/* Format gyroscope data */
	DataStruct->Gyroscope_Raw.x = (int16_t)(data[8] << 8 | data[9]);
	DataStruct->Gyroscope_Raw.y = (int16_t)(data[10] << 8 | data[11]);
	DataStruct->Gyroscope_Raw.z = (int16_t)(data[12] << 8 | data[13]);

	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_FastCallib(TM_MPU6050_t* DataStruct) {
	#define MAX_ACC_ERR		2
	#define MAX_GYRO_ERR	2

	AxesValues_t Accelerometer_Mean;
	AxesValues_t Gyroscope_Mean;

	TM_MPU6050_EnableInterrupts(DataStruct);

	ZeroAxes(&DataStruct->Accelerometer_Offset);
	ZeroAxes(&DataStruct->Gyroscope_Offset);

	do{
		TM_MPU6050_CalcMean(DataStruct, 500, 100, &Accelerometer_Mean, 0);
		SubstractAxes(&DataStruct->Accelerometer_Offset, &Accelerometer_Mean,
				&DataStruct->Accelerometer_Offset);
	}while((Accelerometer_Mean.x > MAX_ACC_ERR) || (Accelerometer_Mean.y > MAX_ACC_ERR) || (Accelerometer_Mean.z > MAX_ACC_ERR));

	do{
		TM_MPU6050_CalcMean(DataStruct, 500, 100, &Gyroscope_Mean, 1);
		SubstractAxes(&DataStruct->Gyroscope_Offset, &Gyroscope_Mean,
				&DataStruct->Gyroscope_Offset);
	}while((Gyroscope_Mean.x > MAX_ACC_ERR) || (Gyroscope_Mean.y > MAX_ACC_ERR) || (Gyroscope_Mean.z > MAX_ACC_ERR));

	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_GeneralCallib(TM_MPU6050_t* DataStruct){
	//wait 5 seconds for max axis values
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_CalcMean(TM_MPU6050_t* DataStruct, uint16_t NumberOfValidSamples, uint16_t NumberOfInvalidSamples, AxesValues_t * Mean, uint8_t Device) {
	TM_MPU6050_Interrupt_t	MPU6050_Interrupts;
	ZeroAxes(Mean);
	for(int i=0; i < NumberOfInvalidSamples; i++){
		while(!MPU6050_Interrupts.F.DataReady){
			TM_MPU6050_ReadInterrupts(DataStruct, &MPU6050_Interrupts);
		}
		TM_MPU6050_ReadAccelerometer(DataStruct);
		TM_MPU6050_ReadGyroscope(DataStruct);
	}
	switch(Device){

	case 0:
		for(int i=0; i < NumberOfValidSamples; i++){
			while(!MPU6050_Interrupts.F.DataReady){
				TM_MPU6050_ReadInterrupts(DataStruct, &MPU6050_Interrupts);
			}
			TM_MPU6050_ReadAccelerometer(DataStruct);
			AddAxesToAxis(&DataStruct->Accelerometer_Offset, &DataStruct->Accelerometer_Raw, Mean);
		}
		break;

	case 1:
		for(int i=0; i < NumberOfValidSamples; i++){
			while(!MPU6050_Interrupts.F.DataReady){
				TM_MPU6050_ReadInterrupts(DataStruct, &MPU6050_Interrupts);
			}
			TM_MPU6050_ReadGyroscope(DataStruct);
			AddAxesToAxis(&DataStruct->Gyroscope_Offset, &DataStruct->Gyroscope_Raw, Mean);
		}
		break;
	}
	DivideAxes(Mean, NumberOfValidSamples);
	Mean->x = (int32_t) Mean->x;
	Mean->y = (int32_t) Mean->y;
	Mean->z = (int32_t) Mean->z;

	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

void SubstractAxes(AxesValues_t * StructToAdd_1, AxesValues_t * StructToAdd_2, AxesValues_t * ResultStruct){
	ResultStruct->x = (StructToAdd_1->x - StructToAdd_2->x);
	ResultStruct->y = (StructToAdd_1->y - StructToAdd_2->y);
	ResultStruct->z = (StructToAdd_1->z - StructToAdd_2->z);
}

void AddAxesWithMultiplication(AxesValues_t * StructToAdd_1, AxesValues_t * StructToAdd_2, AxesValues_t * ResultStruct, double Multiplier){
	ResultStruct->x = (StructToAdd_1->x + StructToAdd_2->x) * Multiplier;
	ResultStruct->y = (StructToAdd_1->y + StructToAdd_2->y) * Multiplier;
	ResultStruct->z = (StructToAdd_1->z + StructToAdd_2->z) * Multiplier;
}

void AddAxesToAxis(AxesValues_t * StructToAdd_1, AxesValues_t * StructToAdd_2, AxesValues_t * ResultStruct){
	ResultStruct->x += (StructToAdd_1->x + StructToAdd_2->x);
	ResultStruct->y += (StructToAdd_1->y + StructToAdd_2->y);
	ResultStruct->z += (StructToAdd_1->z + StructToAdd_2->z);
}
void DivideAxes(AxesValues_t * StructToDivide, uint16_t Divisor){
	StructToDivide->x = StructToDivide->x / Divisor;
	StructToDivide->y = StructToDivide->y / Divisor;
	StructToDivide->z = StructToDivide->z / Divisor;
}

void ZeroAxes(AxesValues_t * StructToZero){
	StructToZero->x = 0;
	StructToZero->y = 0;
	StructToZero->z = 0;
}

TM_MPU6050_Result_t TM_MPU6050_CompensateRawData(TM_MPU6050_t* DataStruct) {

	DataStruct->Gyroscope_Compensated_Prev.x = DataStruct->Gyroscope_Compensated.x;
	DataStruct->Gyroscope_Compensated_Prev.y = DataStruct->Gyroscope_Compensated.y;
	DataStruct->Gyroscope_Compensated_Prev.z = DataStruct->Gyroscope_Compensated.z;

	AddAxesWithMultiplication(&DataStruct->Accelerometer_Raw, &DataStruct->Accelerometer_Offset,
			&DataStruct->Accelerometer_Compensated, DataStruct->Accelerometer_Multiply);

	AddAxesWithMultiplication(&DataStruct->Gyroscope_Raw, &DataStruct->Gyroscope_Offset,
			&DataStruct->Gyroscope_Compensated, DataStruct->Gyroscope_Multiply);

	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

TM_MPU6050_Result_t TM_MPU6050_CalcGyroAcc(TM_MPU6050_t* DataStruct, double Time_s) {

	DataStruct->Gyroscope_Acceleration.x = (DataStruct->Gyroscope_Compensated.x - DataStruct->Gyroscope_Compensated_Prev.x)/Time_s;
	DataStruct->Gyroscope_Acceleration.y = (DataStruct->Gyroscope_Compensated.y - DataStruct->Gyroscope_Compensated_Prev.y)/Time_s;
	DataStruct->Gyroscope_Acceleration.z = (DataStruct->Gyroscope_Compensated.z - DataStruct->Gyroscope_Compensated_Prev.z)/Time_s;

	/* Return OK */
	return TM_MPU6050_Result_Ok;
}


TM_MPU6050_Result_t TM_MPU6050_ConvToRad(TM_MPU6050_t* DataStruct) {

	DataStruct->Gyroscope_Acceleration_Rad.x = DataStruct->Gyroscope_Acceleration.x * 0.01745;//2pi/360
	DataStruct->Gyroscope_Acceleration_Rad.y = DataStruct->Gyroscope_Acceleration.y * 0.01745;//2pi/360
	DataStruct->Gyroscope_Acceleration_Rad.z = DataStruct->Gyroscope_Acceleration.z * 0.01745;//2pi/360
	/* Return OK */
	return TM_MPU6050_Result_Ok;
}

