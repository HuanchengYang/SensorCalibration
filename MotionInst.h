#ifndef MotionInst_h
#define MotionInst_h

#include <stdint.h>
#include <unistd.h>
#include "MPU6050.h"
#include <string>
#include <iostream>

using namespace std;

class MotionInst
{

public:
	MotionInst();
	~MotionInst();
	
	void config_DLPF();

	int current_DLPF_setting();

	int FIFOcount();
	void resetFIFO();
	bool FIFOOverflowStatus();

	void configFIFO();

	void testFIFOconfig();
		
	void resetSensors();
	
	void calibrate();
	
	bool getCalibrationOffsetDelta( int16_t *accelXOffset, int16_t *accelYOffset, int16_t *accelZOffset,
							int16_t *gyroXOffset, int16_t *gyroYOffset, int16_t *gyroZOffset );

	bool average( uint16_t number, uint16_t wait_ms,
					int16_t *avgXA, int16_t *avgYA, int16_t *avgZA,
					int16_t *avgXG, int16_t *avgYG, int16_t *avgZG,
					bool loadCurrent = FALSE );
	
	bool checkTemperature();
	
	double getAccelInG( int16_t accel );
	double getGyroInDegPerS( int16_t gyro );
	
	void getMotionStamped( double *captTime, double *xa, double *ya, double *za, double *xg, double *yg, double *zg );
	
	void displayAccelFSSelection();
	void selectAccelFSSelection();
	
	void displayGyroFSSelection();
	void selectGyroFSSelection();
	
	void setSamplesPerSecond(int newRate);
	void displaySamplesPerSecond();
	void selectSamplesPerSecond();

	void displayDeviceID();
	void changeDeviceID();
	
	double	samplesPerSecond = 250;
	std::string	deviceID;
			
	void		writeINI();
	void		parseINI();

	int16_t	calibXAOffset = 0;
	int16_t	calibYAOffset = 0;
	int16_t	calibZAOffset = 0;
	int16_t	calibXGOffset = 0;
	int16_t	calibYGOffset = 0;
	int16_t	calibZGOffset = 0;

	uint8_t	accelFSSelection = MPU6050_ACCEL_FS_2;
	uint8_t	gyroFSSelection = MPU6050_GYRO_FS_250;
				
private:
	MPU6050		mpu;
	bool		isReady;
	bool		cycleCalibrate;
	
	int16_t	lastXA, lastYA, lastZA, lastXG, lastYG, lastZG;
};


#endif //MotionInst_h
