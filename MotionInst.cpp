#include "MotionInst.h"

#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <sys/time.h>
#include <ostream>
#include <fstream>
#include <cstring>
#include "INIReader.h"

#define CALIBRATION_ACCUM_COUNT 250
#define CALIB_READ_DELAY_MS 5
#define CALIBRATION_INI "inst.ini"

using namespace std;

typedef struct {
	uint8_t		fsSel;
	double		sensFactor;
	char		fsDesc[40];
} FS_SEL;

FS_SEL gyroFS[] = {
	{MPU6050_GYRO_FS_250,	131.072,	"250 degree per second\0"},
	{MPU6050_GYRO_FS_500,	65.536,		"500 degree per second\0"},
	{MPU6050_GYRO_FS_1000,	32.768,		"1000 degree per second\0"},
	{MPU6050_GYRO_FS_2000,	16.384,		"2000 degree per second\0"},
};

FS_SEL accelFS[] = {
	{MPU6050_ACCEL_FS_2,	16384.0,	"+/- 2g\0"},
	{MPU6050_ACCEL_FS_4,	8192.0,		"+/- 4g\0"},
	{MPU6050_ACCEL_FS_8,	4096.0,		"+/- 8g\0"},
	{MPU6050_ACCEL_FS_16,	2048.0,		"+/- 16g\0"},
};

#define GYRO_OFFSET_FS	MPU6050_GYRO_FS_1000
#define ACCEL_OFFSET_FS	MPU6050_ACCEL_FS_16

MotionInst::MotionInst()
{	
	// initialize device
    #if DEBUG
		printf("Initializing MPU6050\n");
	#endif
    mpu.initialize();

    // verify connection
    #if DEBUG
		printf("Test connection to MPU6050\n");
	#endif
	
    isReady = mpu.testConnection();
    #if DEBUG
		printf(isReady ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");
	#endif
	
	deviceID = "INST";
	
	if ( isReady )
	{
		// Make sure sensor in known state
		resetSensors();
	}
	
	// Load INI File
	this->parseINI();

	this->setSamplesPerSecond(this->samplesPerSecond);
}

MotionInst::~MotionInst()
{
	;
}

int MotionInst::current_DLPF_setting() { return (int)mpu.getDLPFMode(); }

void MotionInst::config_DLPF(){
	char temp;
	
	cout << "\n\n\t\tDLPF Configuration Menu\n\n";
 	cout << "          |   ACCELEROMETER    |           GYROSCOPE		    \n";
 	cout << " DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate \n";
 	cout << " ---------+-----------+--------+-----------+--------+-------------\n";
 	cout << " 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz	    \n";
 	cout << " 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz	    \n";
 	cout << " 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz	    \n";
 	cout << " 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz	    \n";
 	cout << " 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz	    \n";
 	cout << " 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz	    \n";
 	cout << " 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz	    \n";
 	cout << " 7        |   -- Reserved --   |   -- Reserved --   | Reserved\n\n";

	cout << "Current DLPF Setting: " << (int) mpu.getDLPFMode() << endl << endl;

	cout << "Enter new value for DLPF_CFG [0-6]: ";

	temp = getchar();
	char cTemp[20];
	cin.getline( cTemp, 20 );
int cfg = temp - '0';
	
	if( cfg > 6 || cfg < 0 ){
		cout << "Entry invald - out of range.";
		cout <<	"\n\n";
	}else{
	mpu.setDLPFMode( cfg );
	cout << "\nNew DLPF Setting: " << (int) mpu.getDLPFMode();
	this->setSamplesPerSecond(samplesPerSecond);
	}
}

void MotionInst::configFIFO() {
	// Allow X,Y,Z values for accel and gyro to be
	// written to FIFO buffer.
	mpu.setFIFOEnabled(true);
	mpu.setAccelFIFOEnabled(true);
	mpu.setZGyroFIFOEnabled(true);
	mpu.setYGyroFIFOEnabled(true);
	mpu.setXGyroFIFOEnabled(true);
	mpu.setTempFIFOEnabled(false);

	// Allow MPU6050 to raise an interrupt
	// when FIFO buffer overflows.
	mpu.setIntFIFOBufferOverflowEnabled(true);
	
	// Reset FIFO.
	mpu.resetFIFO();
}


void MotionInst::testFIFOconfig() {
	mpu.setFIFOEnabled(true);
	mpu.setZGyroFIFOEnabled(true);
	mpu.setIntFIFOBufferOverflowEnabled(true);
	mpu.resetFIFO();
	mpu.setRate(31);

	cout << "FIFO configuration to follow." << endl << endl;

	cout << "Sample rate raw: " << (int)mpu.getRate() << " In Hz: " << 8000.0 / (1 + mpu.getRate()) << endl;
	cout << "FIFO enabled: " << mpu.getFIFOEnabled() << endl;
	cout << "FIFO z gyro enabled: " << mpu.getZGyroFIFOEnabled() << endl << endl;
	cout << "FIFO accel enabled: " << mpu.getAccelFIFOEnabled() << endl;
	cout << "FIFO x, y gyro enabled: " << mpu.getYGyroFIFOEnabled() << ", " << mpu.getXGyroFIFOEnabled() << endl;
	cout << "FIFO overflow INT enabled: " << mpu.getIntFIFOBufferOverflowEnabled() << "Status: " << mpu.getIntFIFOBufferOverflowStatus() << endl;

	//while(true){
	for (int i = 0; i < 50; i++) {
		int count = mpu.getFIFOCount();
		cout << "Count: " << count << endl;

		if (count > 0) {
			uint16_t zGyroH = (uint16_t)mpu.getFIFOByte();
			uint16_t zGyroL = (uint16_t)mpu.getFIFOByte();

			uint16_t zGyroBytes = (zGyroH << 8) | zGyroL;
			cout << "Z gyro: " << getGyroInDegPerS(zGyroBytes) << endl;
		}

	}
}

void MotionInst::resetFIFO() {
	mpu.resetFIFO();
}

int MotionInst::FIFOcount() {
	return mpu.getFIFOCount();
}

bool MotionInst::FIFOOverflowStatus() {
	return mpu.getIntFIFOBufferOverflowStatus();
}

void MotionInst::resetSensors()
{
	if ( !isReady ) return;
	mpu.setDLPFMode( 0 );
	mpu.resetSensors();
}

void MotionInst::calibrate()
{
	int16_t tempXAOffset = calibXAOffset;
	int16_t tempYAOffset = calibYAOffset;
	int16_t tempZAOffset = calibZAOffset;
	int16_t tempXGOffset = calibXGOffset;
	int16_t tempYGOffset = calibYGOffset;
	int16_t tempZGOffset = calibZGOffset;
	
	int16_t iTemp;
	double fTemp;
	
	
	
	

	// Set parameters based on configuration
	mpu.setFullScaleAccelRange( accelFSSelection );
	mpu.setFullScaleGyroRange( gyroFSSelection );
	
	// set last values high so that it will be definitely improved
	lastXA = lastYA = lastZA = lastXG = lastYG = lastZG = 0x8000;
	
	cout << "\n\n------------Calibration--------------\n\n";
	cout << "Pressing X stops calibration.\n";
	
	uint16_t cycleCount = 0;
	
	cout << "Checking if temperature is stable";
	
	
	cycleCalibrate = true;
	
	while ( this->cycleCalibrate )
	{
		cycleCount++;
		cout << "\n - Cycle #" << cycleCount << '\n';
		
		getCalibrationOffsetDelta( &tempXAOffset, &tempYAOffset, &tempZAOffset,
			&tempXGOffset, &tempYGOffset, &tempZGOffset );
			
		if (cycleCount > 10) cycleCalibrate = false;
	}
	
	cout << "\nFinal Average Reading\n";
	iTemp = lastXA;
	fTemp = (double)iTemp / 16384; // in g based on +/-2g or 16384LSB/g
	cout << "Average X Accel = " << fTemp << "g.\n";
	
	iTemp = lastYA;
	fTemp = (double)iTemp / 16384; // in g based on +/-2g or 16384LSB/g
	cout << "Average Y Accel = " << fTemp << "g.\n";

	iTemp = lastZA;
	if ( iTemp > 0 ) iTemp -= 16384;
	else iTemp += 16384;
	fTemp = (double)iTemp / 16384; // in g based on +/-2g or 16384LSB/g
	cout << "Average Z Accel (gravity removed) = " << fTemp << "g.\n";

	iTemp = lastXG;
	fTemp = (double)iTemp / 131; // based on +/- 250 degrees/s or 131LSB / degree/s
	cout << "Average X GYRO = " << fTemp << "degrees/s.\n";		

	iTemp = lastYG;
	fTemp = (double)iTemp / 131; // based on +/- 250 degrees/s or 131LSB / degree/s
	cout << "Average Y GYRO = " << fTemp << "degrees/s.\n";	

	iTemp = lastZG;
	fTemp = (double)iTemp / 131; // based on +/- 250 degrees/s or 131LSB / degree/s
	cout << "Average Z GYRO = " << fTemp << "degrees/s.\n";	
		
	cout << "\n\nDo you want to commit these calibration values to sensor? (Y/N): ";
	char c = 0x00;
	while( 1 )
	{
		c = getchar();
		char cTemp[20];
		cin.getline( cTemp, 20 );
		
		if (c == 'y' || c == 'Y')
		{
			calibXAOffset = tempXAOffset;
			calibYAOffset = tempYAOffset;
			calibZAOffset = tempZAOffset;
			calibXGOffset = tempXGOffset;
			calibYGOffset = tempYGOffset;
			calibZGOffset = tempZGOffset;

			break;
		}
		if (c == 'n' || c == 'N' )
			break;
	}
	
	// Set current offsets to values passed
	mpu.setXAccelOffset( calibXAOffset );
	mpu.setYAccelOffset( calibYAOffset );
	mpu.setZAccelOffset( calibZAOffset );
	mpu.setXGyroOffsetUser( calibXGOffset );
	mpu.setYGyroOffsetUser( calibYGOffset );
	mpu.setZGyroOffsetUser( calibZGOffset );

}

bool MotionInst::getCalibrationOffsetDelta(
	int16_t *accelXOffset, int16_t *accelYOffset, int16_t *accelZOffset,
	int16_t *gyroXOffset, int16_t *gyroYOffset, int16_t *gyroZOffset )
{
	if ( !isReady ) return false;
	
	// Store current full scale range
	uint8_t currentFSA = mpu.getFullScaleAccelRange();
	uint8_t currentFSG = mpu.getFullScaleGyroRange();
	
	// Set to highest sensitivity to calculate offset delta
	mpu.setFullScaleAccelRange( 0 ); 	// +/-2g where 16384 per g
	mpu.setFullScaleGyroRange( 0 ); 	// +/-250 degrees/s where 131 per degree/s

	// Delay for a bit just to make sure settle
	std::this_thread::sleep_for( std::chrono::milliseconds(100));
		
	#if DEBUG
		cout << "Starting Offsets - ";
		cout << std::hex << "Accel X:" << *accelXOffset << " Accel Y:" << *accelYOffset;
		cout << " Accel Z:" << *accelZOffset << "  Gyro X:" << *gyroXOffset << " Gyro Y:" << *gyroYOffset;
		cout << " Gyro z:" << *gyroZOffset << std::dec << '\n';
	#endif

	// Set current offsets to values passed
	mpu.setXAccelOffset( *accelXOffset );
	mpu.setYAccelOffset( *accelYOffset );
	mpu.setZAccelOffset( *accelZOffset );
	mpu.setXGyroOffsetUser( *gyroXOffset );
	mpu.setYGyroOffsetUser( *gyroYOffset );
	mpu.setZGyroOffsetUser( *gyroZOffset );

	// Average a bunch of readings
	int16_t avgXA, avgYA, avgZA, avgXG, avgYG, avgZG;
	this->average( CALIBRATION_ACCUM_COUNT, CALIB_READ_DELAY_MS, &avgXA, &avgYA,
					&avgZA, &avgXG, &avgYG, &avgZG );
	
	// Determine new offset values
	double fTemp;
	int16_t iTemp;
	
	// Accelerometer X
	if (abs(avgXA) < abs(lastXA))
	{
		lastXA = avgXA;
		iTemp = -avgXA;
		fTemp = (double)iTemp / 16384; // in g based on +/-2g or 16384LSB/g
		cout << "Average X Accel = " << fTemp << "g.\n";
		*accelXOffset += (int16_t)(fTemp * accelFS[ACCEL_OFFSET_FS].sensFactor); // based on +/-16g or 2048LSB / g
	}
	else
		cout << "Average X Accel no improvement\n";

	// Accelerometer Y
	if (abs(avgYA) < abs(lastYA))
	{
		lastYA = avgYA;
		iTemp = -avgYA; // get offset delta
		fTemp = (double)iTemp / 16384; // in g based on +/-2g or 16384LSB/g
		cout << "Average Y Accel = " << fTemp << "g.\n";
		*accelYOffset += (int16_t)(fTemp * accelFS[ACCEL_OFFSET_FS].sensFactor); // based on +/-16g or 2048LSB / g
	}
	else
		cout << "Average Y Accel no improvement\n";
	
	// Accelerometer Z
	if (abs(avgZA) < abs(lastZA))
	{
		lastZA = avgZA;
		iTemp = -avgZA; // get offset delta
		if ( iTemp < 0 )	// remove gravity
			iTemp += 16384;
		else
			iTemp -= 16384;
		fTemp = (double)iTemp / 16384; // in g based on +/-2g or 16384LSB/g
		cout << "Average Z Accel (gravity removed) = " << fTemp << "g.\n";
		*accelZOffset += (int16_t)(fTemp * accelFS[ACCEL_OFFSET_FS].sensFactor); // based on +/-16g or 2048LSB / g
	}
	else
		cout << "Average Z Accel no improvement\n";

	// Gyro X
	if (abs(avgXG) < abs(lastXG))
	{
		lastXG = avgXG;
		iTemp = -(int16_t)avgXG; // get offset delta	void displayAccelFSSelection();
	void selectAccelFSSelection();
	
	void displayGyroFSSelection();
	void selectGyroFSSelection();
		fTemp = (double)iTemp / 131; // based on +/- 250 degrees/s or 131LSB / degree/s
		cout << "Average X GYRO = " << fTemp << "degrees/s.\n";
		*gyroXOffset += (int16_t)(fTemp * 32.768);	// based on +/-1000 degrees/s or 32.768LSB/degree/s
	}
	else
		cout << "Average X GYRO no improvement\n";
		
	// Gyro Y
	if (abs(avgYG) < abs(lastYG))
	{
		lastYG = avgYG;
		iTemp = -(int16_t)avgYG; // get offset delta
		fTemp = (double)iTemp / 131; // based on +/- 250 degrees/s or 131LSB / degree/s
		cout << "Average Y GYRO = " << fTemp << "degrees/s.\n";
		*gyroYOffset += (int16_t)(fTemp * 32.768);	// based on +/-1000 degrees/s or 32.768LSB/degree/s
	}
	else
		cout << "Average Y GYRO no improvement\n";
		
	// Gyro Z
	if (abs(avgZG) < abs(lastZG))
	{
		lastZG = avgZG;
		iTemp = -(int16_t)avgZG; // get offset delta
		fTemp = (double)iTemp / 131; // based on +/- 250 degrees/s or 131LSB / degree/s
		cout << "Average Z GYRO = " << fTemp << "degrees/s.\n";
		*gyroZOffset += (int16_t)(fTemp * 32.768);	// based on +/-1000 degrees/s or 32.768LSB/degree/s
	}
	else
		cout << "Average Z GYRO no improvement\n";
		
	#if DEBUG
		cout << "New Suggested Offsets - ";
		cout << std::hex << "Accel X:" << *accelXOffset << " Accel Y:" << *accelYOffset;
		cout << " Accel Z:" << *accelZOffset << "  Gyro X:" << *gyroXOffset << " Gyro Y:" << *gyroYOffset;
		cout << " Gyro z:" << *gyroZOffset << std::dec << '\n';
	#endif
	
	// Return Full Scale Range to previous values
	mpu.setFullScaleAccelRange( currentFSA );
	mpu.setFullScaleGyroRange( currentFSG );
	
	return true;
}

bool MotionInst::average( uint16_t number, uint16_t wait_ms,
					int16_t *avgXA, int16_t *avgYA, int16_t *avgZA,
					int16_t *avgXG, int16_t *avgYG, int16_t *avgZG,
					bool loadCurrent )
{

	int16_t tempXA, tempYA, tempZA, tempXG, tempYG, tempZG;
	int64_t accumXA = 0, accumYA = 0, accumZA = 0, accumXG = 0, accumYG = 0, accumZG = 0;
	uint16_t accumCount;

	if (loadCurrent)
	{	
		// Set current offsets to values loaded or default
		mpu.setXAccelOffset( calibXAOffset );
		mpu.setYAccelOffset( calibYAOffset );
		mpu.setZAccelOffset( calibZAOffset );
		mpu.setXGyroOffsetUser( calibXGOffset );
		mpu.setYGyroOffsetUser( calibYGOffset );
		mpu.setZGyroOffsetUser( calibZGOffset );

		cout << "Loaded Offsets - ";
		cout << std::hex << "Accel X:" << calibXAOffset << " Accel Y:" << calibYAOffset;
		cout << " Accel Z:" << calibZAOffset << "  Gyro X:" << calibXGOffset << " Gyro Y:" << calibYGOffset;
		cout << " Gyro z:" << calibZGOffset << std::dec << '\n';
	}
	
	// Get a bunch of readings and accumulate
	#if DEBUG
		cout << "Accumulating : Please Wait\n";
	#endif
	for ( accumCount = 0; accumCount < number; accumCount++ )
	{
		// Fetch from sensor
		mpu.getMotion6( &tempXA, &tempYA, &tempZA, &tempXG, &tempYG, &tempZG );

		// Add to accumulation
		accumXA += tempXA;
		accumYA += tempYA;
		accumZA += tempZA;
		accumXG += tempXG;
		accumYG += tempYG;
		accumZG += tempZG;
		
		// Delay for a bit to make sure grabbing a new value set
		std::this_thread::sleep_for( std::chrono::milliseconds(wait_ms));
	}
	
	#if DEBUG
		cout << "\nAccumulated " << accumCount << " records.\n";
	#endif

	*avgXA = (int16_t)(accumXA / (int64_t)accumCount );
	*avgYA = (int16_t)(accumYA / (int64_t)accumCount );
	*avgZA = (int16_t)(accumZA / (int64_t)accumCount );
	*avgXG = (int16_t)(accumXG / (int64_t)accumCount );
	*avgYG = (int16_t)(accumYG / (int64_t)accumCount );
	*avgZG = (int16_t)(accumZG / (int64_t)accumCount );

	return true;
}

double MotionInst::getAccelInG( int16_t accel )
{
	return (double)accel / accelFS[accelFSSelection].sensFactor;
}

double MotionInst::getGyroInDegPerS( int16_t gyro )
{
	return (double)gyro / gyroFS[gyroFSSelection].sensFactor;
}

void MotionInst::getMotionStamped( double *captTime, double *xa, double *ya, double *za, double *xg, double *yg, double *zg )
{
	uint16_t ixa, iya, iza, ixg, iyg, izg;
	uint8_t buf[12];

	*captTime += 1.0 / samplesPerSecond;
	// Read the next 6 values from the FIFO.
	// FIFO ordered in ascending register number.
	
	mpu.getFIFOBytes(buf, 12);
	
	ixa = ((buf[0] << 8) | (buf[1]));
	iya = ((buf[2] << 8) | (buf[3]));
	iza = ((buf[4] << 8) | (buf[5]));

	ixg = ((buf[6] << 8) | (buf[7]));
	iyg = ((buf[8] << 8) | (buf[9]));
	izg = ((buf[10] << 8) | (buf[11]));

	*xa = getAccelInG(ixa);
	*ya = getAccelInG(iya);
	*za = getAccelInG(iza);

	*xg = getGyroInDegPerS(ixg);
	*yg = getGyroInDegPerS(iyg);
	*zg = getGyroInDegPerS(izg);
}

void MotionInst::displayAccelFSSelection()
{
	cout << (int)accelFSSelection << " : " << accelFS[accelFSSelection].fsDesc;
}

void MotionInst::selectAccelFSSelection()
{
	cout << "\n-------------Select Acceleration Full Scale--------------\n\n";
	for ( int count = 0; count < 4; count++ )
	{
		cout << " [" << count << "] - ";
		cout << accelFS[count].fsDesc << '\n';
	}
	cout << "Make selection ([x] exits): ";
	
	bool valid = false;
	
	while ( !valid )
	{
		char c = getchar();
		char cTemp[20];
		cin.getline(cTemp, 20);
		int select = c - '0';
		if ( select >= 0 && select < 4 )
		{
			accelFSSelection = (uint8_t)select;
			break;
		}
		if ( c == 'x' || c == 'X' )
			break;
	}
}
	
void MotionInst::displayGyroFSSelection()
{
	cout << (int)gyroFSSelection << " : " << gyroFS[gyroFSSelection].fsDesc;
}

void MotionInst::selectGyroFSSelection()
{
	cout << "\n-------------Select Gyro Full Scale--------------\n\n";
	for ( int count = 0; count < 4; count++ )
	{
		cout << " [" << count << "] - ";
		cout << gyroFS[count].fsDesc << '\n';
	}
	cout << "Make selection ([x] exits): ";
	
	bool valid = false;
	
	while ( !valid )
	{
		char c = getchar();
		char cTemp[20];
		cin.getline(cTemp, 20);
		
		int select = c - '0';
		if ( select >= 0 && select < 4 )
		{
			gyroFSSelection = (uint8_t)select;
			break;
		}
		if ( c == 'x' || c == 'X' )
			break;
	}
}

void MotionInst::displaySamplesPerSecond()
{
	cout << samplesPerSecond << endl;
}

void MotionInst::setSamplesPerSecond( int newRate ) {
	if (newRate >= 32 && newRate <= 1000) {
		samplesPerSecond = newRate;
		/*
		Sample rate of MPU determined using:
		rate = GyroRate / (1 + DIV)
		where rate is the desired sample rate (Hz), Gyro rate is 8000Hz if DLPF = 0 or 1000 if DLPF != 0,
		and DIV is a constant stored in an MPU register.
		*/
		double GyroRate = 1000;
		if ( current_DLPF_setting() == 0) {
//			cout << "GyroRate is 8000" << endl;
			GyroRate = 8000;
		}
		else {
//			cout << "GyroRate is 1000" << endl;
		}
		uint8_t mpuDIV = (uint8_t)( (GyroRate / (double)samplesPerSecond) - 1.0 );

//		cout << "mpuDIV: " << (int)mpuDIV << endl;

		mpu.setRate(mpuDIV);
		// Check for truncation errors.
		samplesPerSecond = (GyroRate / ((double)mpu.getRate() + 1.0));
	}
}

void MotionInst::selectSamplesPerSecond()
{
	cout << "\n----------Select Samples Per Second ------------------\n\n";
	cout << "Select between 32 and 1000 samples per second :";
	char charactersIn[5];
	cin.getline( charactersIn, 5 );
	
	int i = atoi(charactersIn);
	
	setSamplesPerSecond(i);
	cout << "Sample rate set to: " << samplesPerSecond << endl;
}

void MotionInst::displayDeviceID()
{
	cout << deviceID;
}

void MotionInst::changeDeviceID()
{
	std::string newID;
	char cTemp[20];
		
	cout << "\n\n Enter up to 4 characters for device ID: ";
	cin >> newID;
	cin.getline(cTemp, 20); // flush any garbage
	newID.resize(4); // make sure only 4 characters
	
	cout << "\n You entered " << newID << ". You sure? (y/n): ";
	char c = getchar();
	cin.getline(cTemp, 20);

	if ( c == 'n' || c == 'N' ) return;
	
	if ( c == 'Y' || c == 'y' )
		deviceID =  newID;
}

void MotionInst::writeINI()
{
	std::filebuf iniFile;
	iniFile.open( CALIBRATION_INI, std::ios::out );
	std::ostream iniStream(&iniFile);

	iniStream << "; INST.INI contains configuration and calibation info for instruments";

	// CONFIG
	iniStream << "\n\n";
	iniStream << "[CONFIG]\n";
	iniStream << "DeviceID = " << deviceID << '\n';
	iniStream << "SamplesPerSecond = " << (int)samplesPerSecond << '\n';
	iniStream << "AccelFSSelection = " << (int)accelFSSelection << '\n';
	iniStream << "GyroFSSelection = " << (int)gyroFSSelection << '\n';

	// CALIBRATION
	iniStream << "\n\n";
	iniStream << "[CALIBRATION]\n";
	iniStream << "XAOFFSET = " << calibXAOffset << '\n';
	iniStream << "YAOFFSET = " << calibYAOffset << '\n';
	iniStream << "ZAOFFSET = " << calibZAOffset << '\n';
	iniStream << "XGOFFSET = " << calibXGOffset << '\n';
	iniStream << "YGOFFSET = " << calibYGOffset << '\n';
	iniStream << "ZGOFFSET = " << calibZGOffset << '\n';
	
	// Close and save the file
	iniFile.close();
}

void MotionInst::parseINI()
{
	INIReader reader( CALIBRATION_INI );
	
	if ( reader.ParseError() < 0 )
	{
		cout << "\n\n****WARNING: Could not parse " << CALIBRATION_INI << "\n\n";
		return;
	}
	
	cout << "\n\n--->" << CALIBRATION_INI << " values read.\n\n";

	// CONFIG
	deviceID = reader.Get( "CONFIG", "DeviceID", "INST" );
	samplesPerSecond = reader.GetInteger( "CONFIG", "SamplesPerSecond", 250 );
	accelFSSelection = reader.GetInteger( "CONFIG", "AccelFSSelection", 0 );
	gyroFSSelection = reader.GetInteger( "CONFIG", "GyroFSSelection", 0 );
	
	// CALIBRATION
	calibXAOffset = (int16_t)reader.GetInteger( "CALIBRATION", "XAOFFSET", 0 );
	calibYAOffset = (int16_t)reader.GetInteger( "CALIBRATION", "YAOFFSET", 0 );
	calibZAOffset = (int16_t)reader.GetInteger( "CALIBRATION", "ZAOFFSET", 0 );
	calibXGOffset = (int16_t)reader.GetInteger( "CALIBRATION", "XGOFFSET", 0 );
	calibYGOffset = (int16_t)reader.GetInteger( "CALIBRATION", "YGOFFSET", 0 );
	calibZGOffset = (int16_t)reader.GetInteger( "CALIBRATION", "ZGOFFSET", 0 );
}

bool MotionInst::checkTemperature(){
	int16_t temperature1;
	int16_t temperature2;
	bool stable_temperature=false;
	double temperature;
	
	while (!stable_temperature){
	temperature1=mpu.getTemperature();
	std::this_thread::sleep_for( std::chrono::milliseconds(100));
	temperature2=mpu.getTemperature();
	
	if (temperature2==temperature1){
		stable_temperature=true;
		temperature=temperature2/340+36.53;
		cout<<"Temperature is stable now. The current temperature is "<<temperature<<" C \n";
	}
	else{
		cout<<"Current Temperature 1 is"<<temperature1<<" and current temperature 2 is "<< temperature2<<"\n";
		cout<<"Check temperature again in 1s \n";
		std::this_thread::sleep_for( std::chrono::milliseconds(1000));
	}
	}
	return stable_temperature;
	
}
