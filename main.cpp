#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <ostream>
#include <fstream>
#include "MotionInst.h"
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <cstdio>
#include "ntplib.h"
#include <sstream>


#define VERSION "V:170708 - 1.1.0"

MotionInst *inst = 0;

using namespace std;
using namespace std::chrono;

bool runCaptureThread;
bool		ntpSync = false;
double		ntpDelay;
double		ntpOffset;
double		ntpJitter;
char file_name_string[100];
	
void syncTime( int print )
{
	cout << "\n\nPreparing to syncronize timing with NTP server.\n";
	cout << " NOTE: This routine is not robust, thus if NTP server\n";
	cout << "       either not running or not present, system will\n";
	cout << "       hang here.\n\n";
	cout << " Timing is based on NTP protocol. The \"Offset\" value is\n";
	cout << " of interest as it is the time delta from the NTP server\n";
	cout << " which is important to syncronize data betwen two devices\n";
	cout << " sharing same NTP server.\n\n";
	cout << " Two Raspberry Pi devices will drift about 0.05s per 12 hour\n";
	cout << " period from each other in timing (from Remi Bergsma's blog)\n";
	cout << " and thus about 4ms drift an hour. It is recommended that time\n";
	cout << " sync be done on all units at least every hour.\n\n";
	cout << "Are you sure you want to proceed (y/n);";
	
	char c = getchar();
	char cTemp[20];
	cin.getline(cTemp, 20);
	
	if ( c != 'y' && c !='Y' ) return;
	
	startNTP();
	cout << "- Syncronize time with ntp. Process can take a minute.\n";
	while ( !syncNTP(&ntpDelay, &ntpOffset, &ntpJitter, print) );
	cout << "- Sync established - stopping NTP service.\n";
	stopNTP();
	ntpDelay /= 1000.0;
	ntpOffset /= 1000.0;
	ntpJitter /= 1000.0;
	cout << "- Delay (s): " << ntpDelay << "  Offset (s): " << ntpOffset;
	cout << "  Jitter (s):" << ntpJitter << "\n\n";
	
	ntpSync = true;
}

void captureTask(short capturesPerSec)
{
	if ( capturesPerSec < 4 || capturesPerSec > 1000 )
	{
		cout << "There is a maximum of 1000 captures per second.\n\n";
		runCaptureThread = false;
		return;
	}
	
	cout << "\n\n<<<<<<CAPTURE STARTED>>>>>>>>>\n\n";	
	
	uint16_t captures = 0;
	std::filebuf captureFile;
	
	time_t rawtime;
	struct tm *timeinfo;
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	sprintf( file_name_string, "/home/pi/Instrument/Data/%4s-%04d-%02d-%02d_%02d-%02d-%02d.csv", inst->deviceID.data(), timeinfo->tm_year + 1900,
		timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour,
		timeinfo->tm_min, timeinfo->tm_sec );
		
	cout << "<<<<<< Filename: " << file_name_string << " >>>>>>\n";
	captureFile.open( file_name_string, std::ios::out );
	std::ostream captureStream(&captureFile);
	
	// Put in header information
	// Version number and filename
	captureStream << "\"" << VERSION << "\"" << "\n";
	captureStream << "\"" << file_name_string << "\"" << '\n';

	// NTP Details
	captureStream << '\n' << "\"NTP Offset\", ";
	if (ntpSync)
		captureStream << ntpOffset;
	else
		captureStream << "INVALID";
	captureStream << ',';
	captureStream << "\"NTP Delay\", ";
	if (ntpSync)
		captureStream << ntpDelay;
	else
		captureStream << "INVALID";
	captureStream << ',';
	captureStream << "\"NTP Jitter\", ";
	if (ntpSync)
		captureStream << ntpJitter;
	else
		captureStream << "INVALID";
	captureStream << "\n";

	// Calibration Details
	captureStream << "\"Calibration Offsets\", AccelX, " << inst->calibXAOffset << ',';
	captureStream << " AccelY, " << inst->calibYAOffset << ',';
	captureStream << " AccelZ, " << inst->calibZAOffset << ',';
	captureStream << " GyroX, " << inst->calibXGOffset << ',';	
	captureStream << " GyroY, " << inst->calibYGOffset << ',';	
	captureStream << " GyroZ, " << inst->calibZGOffset << '\n';

	// Configuration Details
	captureStream << "\"Configuration\", GyroFS, " << (int)inst->gyroFSSelection << ',';
	captureStream << " AccelFS, " << (int)inst->accelFSSelection << '\n';
	captureStream << "\"Sample Rate\", " << (float)((float)1/(float)inst->samplesPerSecond) << "s," << inst->samplesPerSecond << "Hz" <<"\n\n\n";
	

	double timeDelta=0, xa, ya, za, xg, yg, zg, pitch=0, roll=0;
	
	captureStream << "\"Time Delta\", X Accel, Y Accel, Z Accel, X Gyro, Y Gyro, Z Gyro , pitch , roll\n";
	std::this_thread::yield();
	inst->resetFIFO();
	while ( runCaptureThread )
	{
		try {
			int fifoCount = inst->FIFOcount();
//			cout << "Waiting for FIFO..." << endl;
			if (fifoCount >= 12) {
				if (fifoCount > 1020) {
					cout << "FIFO size: " << fifoCount << endl;
					cout << "FIFO overflow. Sample rate TOO HIGH." << endl << endl;
					runCaptureThread = false;
					break;
				}
//				cout << "Before next motion stamp: " << fifoCount << endl;
				inst->getMotionStamped(&timeDelta, &xa, &ya, &za, &xg, &yg, &zg);
//				fifoCount -= 12;
				inst->ComplementaryFilter(&xa, &ya, &za, &xg, &yg, &zg, &pitch, &roll);
				captureStream << fixed << timeDelta << ',' << xa << ',' << ya << ',' << za << ',';
				captureStream << xg << ',' << yg << ',' << zg << ',' << pitch << ',' << roll << '\n';
				captures++;
				
				if ((int(timeDelta*1000)) % 1000 ==0){
				cout << "Real time data" << "X Accel: "<< xa <<" Y Accel: " << ya << " Z Accel: "<< za <<'\n' << "X Gyro "<<xg<<" Y Gyro "<< yg << " Z Gyro "<< zg <<" pitch " << pitch << " roll " << roll << '\n';
				}
				
			}
		}
		catch(exception& e){
			cout << "Error from I2C bus. Caught and continuing execution (clock stretch bug)." << endl;
		}

	}
	cout << "\n<<<<<<CAPTURE STOPPED>>>>>>>>>\n\n";
	captureFile.close();
}

void copyFileToWAP()
{
	if ( !file_name_string[ 0 ] )
	{
		cout << "No file created yet in this session.\n";
		// Nothing to do
		return;
	}

	cout << "****************************************\n";
	cout << "*** Do you wish to copy file to WAP? ***\n";
	cout << "****************************************\n";
	cout << "copy " << file_name_string << "? (y/n):";
	char c;
	char cTemp[20];
	
	while ( 1 ) {
		c = getchar();
		cin.getline(cTemp, 20);
		
		if ( c == 'n' || c == 'N' ) break;
		
		if ( c == 'y' || c == 'Y' )
		{
			// create system command to copy file
			std::ostringstream command;
			command << "scp " << file_name_string << " pi@192.168.42.1:";
			command << file_name_string;
			
			// Issue the command
			std::string commandStr = command.str();
			
			int i = system( commandStr.c_str() );
			if ( !i )
				cout << "File transferred successfully. \n";
			else
			{
				cout << "\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!\n";
				cout << "File transferred failed in some way\n";
				cout << "You may need to transfer file manually.\n\n";
			}
			break;
		}
	}
}
void menu1()
{
	cout << "\n\nInstrument Menu (" << VERSION << ")\n\n";
	cout << "[0] - DeviceID: ";
	inst->displayDeviceID();
	cout << "\n=================================\n";
	cout << "[1] - Execute Calibration\n";
	cout << "[2] - Check Calibration\n";
	cout << "[3] - Change Gyro FS - ";
	inst->displayGyroFSSelection();
	cout << "\n";
	cout << "[4] - Change Accel FS - ";
	inst->displayAccelFSSelection();
	cout << "\n";
	cout << "[5] - Change Sample Rate per second - ";
	inst->displaySamplesPerSecond();
	cout << "\n";
	cout << "[6] - Configure digital low-pass filter (DLPF) - " << inst->current_DLPF_setting() << endl;	
	cout << "[7] - Save Config\n";
	cout << "[8] - Check Temperature\n";
	

	cout << "\n[C] - Copy File to WAP - " << file_name_string << "\n";
	cout << "\n[T] - Sync NTP Time - " << ((ntpSync) ? "Synced" : "NOT Synced") << '\n';
	cout << "\n[S] - Start Capture\n";
}

void menu2()
{
	
}

int main()
{
	char c;
	bool menu = true;
	bool valid;
	
	// Make sure filename is nulled
	file_name_string[ 0 ] = 0;
	
	// Create the instrument object
	inst = new MotionInst();
	
	// Make sure starting with clean slate
	inst->resetSensors();

	if ( !inst ) return -1;
	
	inst->resetSensors(); // Start with clean slate

	// Set up FIFO buffer.
	inst->configFIFO();
	inst->setSamplesPerSecond(inst->samplesPerSecond);
	runCaptureThread = false;

	while ( menu )
	{
		std::this_thread::yield();
		if (!runCaptureThread) {
			menu1();
			valid = false;
			
			while (!valid)
			{
				cout << "----- Enter selection ([X] exits): ";
				valid = true;
				c = getchar();
				char cTemp[20];
				cin.getline(cTemp, 20);
		
				switch ( c )
				{
					case '0' : // Change device ID
						inst->changeDeviceID();
						break;
						
					case '1' : // Execute Preparing to capture samplesCalibration
						inst->calibrate();
						break;
				
					case '2' : // Check Calibration
						{
							int16_t cxa, cya, cza, cxg, cyg, czg;
							inst->average(250, 5, &cxa, &cya, &cza, &cxg, &cyg, &czg, true);
							cout << "\n----------Average Converted Data-----------\n";
							cout << " Average X Accel = " << inst->getAccelInG(cxa) << '\n';
							cout << " Average Y Accel = " << inst->getAccelInG(cya) << '\n';
							cout << " Average Z Accel = " << inst->getAccelInG(cza) << '\n';
							cout << " Average X GYRO = " << inst->getGyroInDegPerS(cxg) << '\n';
							cout << " Average Y GYRO = " << inst->getGyroInDegPerS(cyg) << '\n';
							cout << " Average Z GYRO = " << inst->getGyroInDegPerS(czg) << "\n\n";
						}
						break;

					case '3' : // Change gryoFS
						inst->selectGyroFSSelection();
						break;
						
					case '4' : // Change accelFS
						inst->selectAccelFSSelection();
						break;
						
					case '5' : // Change Sample Rate per Second
						inst->selectSamplesPerSecond();
						break;
						
					case '6' : //config DLPF
						inst->config_DLPF();
						break;

					case '7' : // Save INI File
						inst->writeINI();
						break;
					case '8' :// check temperature
						inst->checkTemperature();
						break;
					
						
					case 'C' :
					case 'c' :
						copyFileToWAP();
						break;
						
					case 'X' :
						menu = false;
						break;
					
					case 'S' :
					case 's' :
					{
						// first make sure that thread is not already running
						if ( runCaptureThread ) runCaptureThread = false;
						std::this_thread::sleep_for(std::chrono::milliseconds(500)); // make sure thread ends
					
						// start the thread
						runCaptureThread = true;
						std::thread ( captureTask, inst->samplesPerSecond ).detach();
						break;
					}
					
					case 'T' :	// Syncronize time
					case 't' :
						syncTime(1);
						break;
					
					default :
						valid = false;
				}
			}
		}
		else {
			cout << "\n\n[S] - Stop Capture ";
		
			valid = false;		
			while (!valid)
			{
				c = getchar();
				char cTemp[20];
				cin.getline(cTemp, 20);
				valid = true;
				

				switch ( c )
				{
					case 'S' :
					case 's' : 
						runCaptureThread = false;
						std::this_thread::sleep_for(std::chrono::milliseconds(500)); // make sure thread ends
						break;
					
					default :
						valid = false;
				}
			}
		}
	}

	if (inst) delete inst;
}
