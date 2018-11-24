#include "MPU6050.h"
#include "BLE_RN4871.h"
#include <math.h>

extern GYACC_TypeDef GYACCHandle;

void GYACC_Init(I2C_HandleTypeDef* i2cHandle, GYACC_TypeDef* GYACCHandle){
		
	uint8_t I2CTx[6] ;
	uint8_t I2CRx[6] ;
	HAL_StatusTypeDef status;
	
	GYACCHandle->i2cHandle = i2cHandle;
	GYACCHandle->acqStarted = 0;
	GYACCHandle->gyAccStatus = GYACC_OK;
	
	I2C_Config(GYACCHandle->i2cHandle,2);	
	

	
	//======================Initialize GyAcc==================================//
	I2CTx[0]=MPU6050_RA_WHO_AM_I;													// WHO_AM_I() register
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	I2CTx[0]=MPU6050_RA_PWR_MGMT_1;													// PWR_MGMT_1(0x6B) register
	I2CTx[1]=0x00;																					// reset device
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	HAL_Delay(100);
	
	I2CTx[0]=MPU6050_RA_PWR_MGMT_1;													// PWR_MGMT_1(0x6B) register
	I2CTx[1]=0x80;																					// reset device
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	HAL_Delay(100);
	
	I2CTx[0]=MPU6050_RA_PWR_MGMT_1;													// PWR_MGMT_1(0x6B) register
	I2CTx[1]=0x00;																					// temperature disable
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);	
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	HAL_Delay(1);
	HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, MPU6050_RA_ZA_OFFS_L_TC, I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	
	I2CTx[0]=MPU6050_RA_USER_CTRL;													// USER_CTRL(0x6A) register
	I2CTx[1]=0x01;																					// reset signal paths and clear sensor registers
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);	
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
//	I2CTx[0]=MPU6050_RA_SIGNAL_PATH_RESET;									// SIGNAL_PATH_RESET register
//	I2CTx[1]=0x07;																					// Reset Gyro Acelerometer and Temperature sensor paths
//  HAL_I2C_Master_Transmit(&I2CHandle, 0xD0, I2CTx, 2, 2);	
	
	I2CTx[0]=MPU6050_RA_PWR_MGMT_2;													// PWR_MGMT_2(0x6C) register
	I2CTx[1]=0x3F;																					// accelerometer and gyroscope disable
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	I2CTx[0]=MPU6050_RA_SMPLRT_DIV;													// SMPLRT_DIV(0x19) register
	I2CTx[1]=0x63;																					// Sample Rate = Gyroscope Output Rate(1kHz) / (1 + SMPLRT_DIV)
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);
	HAL_Delay(1);	
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	I2CTx[0]=MPU6050_RA_CONFIG;															// CONFIG(0x1A) register
	I2CTx[1]=MPU6050_DLPF_BW_20;														// Low pass BW = 20 Hz
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);	
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	I2CTx[0]=MPU6050_RA_GYRO_CONFIG;												// GYRO_CONFIG(0x1B) register
	I2CTx[1]=MPU6050_GYRO_FS_500<<3;												// Full scale range +/- 500 deg/s
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);	
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	I2CTx[0]=MPU6050_RA_ACCEL_CONFIG;												// ACCEL_CONFIG(0x1C) register
	I2CTx[1]=MPU6050_ACCEL_FS_4<<3;													// Full scale range +/- 4g
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);	
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	I2CTx[0]=MPU6050_RA_INT_PIN_CFG;											// INT_PIN_CFG(0x37) register
	I2CTx[1]=0x80;																				// Enable DataReady interrupt
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);
	HAL_Delay(1);	
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	I2CTx[0]=MPU6050_RA_INT_ENABLE;												// INT_ENABLE(0x38) register
	I2CTx[1]=0x01;																				// Enable DataReady interrupt
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);
	HAL_Delay(1);	
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	/*I2CTx[0]=MPU6050_RA_XG_OFFS_USRH;												
	I2CTx[1]=0x01;																				
	I2CTx[2]=0x2C;	
	HAL_I2C_Master_Transmit(&I2CHandle, 0xD0, I2CTx, 3, 2);	
	HAL_I2C_Mem_Read(&I2CHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 2, 2);
	*/
	//======================Initialize GyAcc==================================//
	
}

void GYACC_Start_Acquisition(GYACC_TypeDef* GYACCHandle){
	
	uint8_t I2CTx[6] ;
	uint8_t I2CRx[6] ;
	HAL_StatusTypeDef status;
	
	if(GYACCHandle->gyAccStatus == GYACC_ERROR) return;
	
	I2CTx[0]=MPU6050_RA_PWR_MGMT_2;													// PWR_MGMT_2(0x6C) register
	I2CTx[1]=0x00;																					// accelerometer and gyroscope enable
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	I2CTx[0]=MPU6050_RA_USER_CTRL;													// USER_CTRL(0x6A) register
	I2CTx[1]=0x01;																					// reset signal paths and clear sensor registers
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);	
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	I2CTx[0]=MPU6050_RA_USER_CTRL;													// USER_CTRL(0x6A) register
	I2CTx[1]=0x01;																					// reset signal paths and clear sensor registers
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);	
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	GYACCHandle->angleIndex = 0;
	GYACCHandle->dataReadyTask = 0;
	GYACCHandle->newDataAvailable = 0;
	GYACCHandle->acqStarted = 1;
	
}
void GYACC_Stop_Acquisition(GYACC_TypeDef* GYACCHandle){
	uint8_t I2CTx[6] ;
	uint8_t I2CRx[6] ;
	HAL_StatusTypeDef status;
	
	I2CTx[0]=MPU6050_RA_PWR_MGMT_2;													// PWR_MGMT_2(0x6C) register
	I2CTx[1]=0x3F;																					// accelerometer and gyroscope disable
	status = HAL_I2C_Master_Transmit(GYACCHandle->i2cHandle, 0xD0, I2CTx, 2, 2);
	HAL_Delay(1);
	status = HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, I2CTx[0], I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 2);
	HAL_Delay(1);
	if(status!=HAL_OK) {
		GYACCHandle->gyAccStatus = GYACC_ERROR;
		GYACCHandle->ErrorNumber = GYACC_INIT1_ERROR;							
	}
	
	GYACCHandle->acqStarted = 0;
		
}
void GYACC_ReadDataFromSensor(GYACC_TypeDef* GYACCHandle){
	uint8_t I2CRx[6] ;
	int16_t x,y,z;
	double xD,yD,zD;
	double rollD, pitchD;
	
	if(HAL_OK==HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, MPU6050_RA_INT_STATUS, I2C_MEMADD_SIZE_8BIT, I2CRx, 1, 1)) {					
		if((I2CRx[0]& 0x01)==0x01) {	
			if(HAL_OK==HAL_I2C_Mem_Read(GYACCHandle->i2cHandle, 0xD0, MPU6050_RA_ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, I2CRx, 6, 1)) {
				x = I2CRx[0];
				x = x << 8;
				x |= I2CRx[1];
				
				y = I2CRx[2];
				y = y << 8;
				y |= I2CRx[3];
				
				z = I2CRx[4];
				z = z << 8;
				z |= I2CRx[5];
				
				//calculate angles according phone specification 
				xD = (double) x;  
				yD = (double) y;	 
				zD = (double) z;	 
				pitchD = atan2(-xD,zD)*57.3;					
				rollD = atan2(yD,sqrt(xD*xD+zD*zD))*57.3;
				GYACCHandle->pitchAngles[GYACCHandle->angleIndex] = (int32_t) pitchD;
				GYACCHandle->rollAngles[GYACCHandle->angleIndex++] = (int32_t) rollD;
				if(GYACCHandle->angleIndex == 5){
					GYACCHandle->angleIndex = 0;
					GYACCHandle->newDataAvailable = 1;
				}
				
			}
			else{
				GYACCHandle->gyAccStatus = GYACC_ERROR;
				GYACCHandle->ErrorNumber = GYACC_READ_DATA1_ERROR;
			}							
		}
		else{
			
		}
	}

}

void GYACC_CalculateAngles(GYACC_TypeDef* GYACCHandle, uint8_t *dataBuffer){
	//ACC:+000+111+222\r\n
	//first coordinate - yaw angle
	//second coordinate - pitch angle
	//third coordinate - roll angle
	
	int32_t roll = 0,pitch = 0;
	uint8_t i;
	for(i=0;i<5;i++){
		roll = roll + GYACCHandle->rollAngles[i];
		pitch = pitch + GYACCHandle->pitchAngles[i];
	}
	roll = roll / 5;
	pitch = pitch / 5;
	if(roll>0){
		dataBuffer[12] = '+';
	}
	else{
		dataBuffer[12] = '-';
		roll = - roll;
	}
	value2DecimalString((char*)dataBuffer+13,roll);
	if(pitch>0){
		dataBuffer[8] = '+';
	}
	else{
		dataBuffer[8] = '-';
		pitch = -pitch;
	}
	value2DecimalString((char*)dataBuffer+9,pitch);
}
void ACC_Interrupt_Callback(void){
	if(GYACCHandle.acqStarted){
		GYACCHandle.dataReadyTask = 1;
	}
}
